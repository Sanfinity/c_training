/*
 * Topic 17: Memory-Mapped I/O & Register Access
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * On real hardware, registers live at fixed physical addresses. Here we SIMULATE
 * them with volatile variables so the same code compiles and runs on a PC.
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>

/* --- Simulated register block (stands in for a real peripheral base) --- */
typedef struct {
    volatile uint32_t CTRL;     /* control    */
    volatile uint32_t STATUS;   /* status     */
    volatile uint32_t SET;      /* write-1-to-set  bits */
    volatile uint32_t CLR;      /* write-1-to-clear bits */
    volatile uint32_t DATA;     /* data       */
} PeriphRegs;

static PeriphRegs sim_periph;   /* pretend this is at, e.g., 0x40000000 */

/* P1: on real HW you'd do: #define PERIPH (*(volatile PeriphRegs*)0x40000000) */
static PeriphRegs *const PERIPH = &sim_periph;

/* P3: set/clear/toggle a bit via read-modify-write. */
void reg_set_bit(volatile uint32_t *reg, unsigned bit)    { *reg |=  (1u << bit); }
void reg_clear_bit(volatile uint32_t *reg, unsigned bit)  { *reg &= ~(1u << bit); }
void reg_toggle_bit(volatile uint32_t *reg, unsigned bit) { *reg ^=  (1u << bit); }
int  reg_read_bit(const volatile uint32_t *reg, unsigned bit) { return (int)((*reg >> bit) & 1u); }

/* P5: write 'value' into the field [msb:lsb] without disturbing other bits. */
void reg_write_field(volatile uint32_t *reg, unsigned lsb, unsigned width, uint32_t value) {
    uint32_t mask = ((width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u)) << lsb;
    *reg = (*reg & ~mask) | ((value << lsb) & mask);
}
uint32_t reg_read_field(const volatile uint32_t *reg, unsigned lsb, unsigned width) {
    uint32_t mask = (width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u);
    return (*reg >> lsb) & mask;
}

/* P4: poll a status bit until set (bounded here so the demo terminates). */
int poll_ready(const volatile uint32_t *status, unsigned bit, int timeout) {
    int t = 0;
    while (!((*status >> bit) & 1u)) {
        if (++t >= timeout) return -1;   /* timeout */
    }
    return t;
}

/* P8: write-1-to-clear a status bit.
 * On real HW you literally WRITE a 1 to the bit and the hardware clears it:
 *     *status = (1u << bit);
 * A plain variable can't reproduce that side effect, so to show the *effect*
 * (the bit becomes 0) we clear it explicitly here. */
void w1c_clear(volatile uint32_t *status, unsigned bit) {
    *status &= ~(1u << bit);   /* emulates the HW's write-1-to-clear result */
}

/* P9: separate SET/CLR registers give "atomic" bit ops (no RMW race). */
void gpio_set(unsigned bit)   { PERIPH->SET = (1u << bit); PERIPH->CTRL |=  (1u << bit); }
void gpio_clear(unsigned bit) { PERIPH->CLR = (1u << bit); PERIPH->CTRL &= ~(1u << bit); }

int main(void) {
    PERIPH->CTRL = 0; PERIPH->STATUS = 0;

    /* P3 */
    reg_set_bit(&PERIPH->CTRL, 4);
    reg_set_bit(&PERIPH->CTRL, 0);
    reg_toggle_bit(&PERIPH->CTRL, 1);
    printf("P3  CTRL after set/toggle = 0x%08X (bit4=%d)\n",
           PERIPH->CTRL, reg_read_bit(&PERIPH->CTRL, 4));

    /* P5: set field [7:4] = 0xA */
    reg_write_field(&PERIPH->CTRL, 4, 4, 0xA);
    printf("P5  field[7:4]           = 0x%X (CTRL=0x%08X)\n",
           reg_read_field(&PERIPH->CTRL, 4, 4), PERIPH->CTRL);

    /* P4: simulate hardware setting the ready bit, then poll. */
    PERIPH->STATUS = (1u << 3);
    printf("P4  poll_ready(bit3)     = %d (>=0 means became ready)\n",
           poll_ready(&PERIPH->STATUS, 3, 100));

    /* P8 */
    w1c_clear(&PERIPH->STATUS, 3);
    printf("P8  after W1C bit3       = %d\n", reg_read_bit(&PERIPH->STATUS, 3));

    /* P9 */
    gpio_set(2);
    printf("P9  gpio_set(2) CTRL bit2= %d\n", reg_read_bit(&PERIPH->CTRL, 2));
    gpio_clear(2);
    printf("P9  gpio_clear(2) bit2   = %d\n", reg_read_bit(&PERIPH->CTRL, 2));
    return 0;
}
