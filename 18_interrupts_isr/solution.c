/*
 * Topic 18: Interrupts & ISRs
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * We SIMULATE interrupts by calling the "ISR" functions directly from main.
 * The patterns (volatile shared state, flag, ring buffer, critical section)
 * are exactly what you use on real hardware.
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>   /* sig_atomic_t */

/* P4: ISR<->main via a volatile flag. */
static volatile sig_atomic_t g_tick_flag = 0;   /* P13: sig_atomic_t for flags */
static volatile uint32_t     g_tick_count = 0;

/* Simulated timer ISR: keep it SHORT - just bump state and set a flag (P3). */
void timer_isr(void) {
    g_tick_count++;
    g_tick_flag = 1;      /* tell main loop work is pending */
}

/* P5: ISR -> main via a lock-free single-producer/single-consumer ring buffer.
 * Producer (ISR) only writes head; consumer (main) only writes tail. */
#define RB_SIZE 8
static volatile uint8_t rb_buf[RB_SIZE];
static volatile uint32_t rb_head = 0, rb_tail = 0;

/* Producer side - called from the "UART RX ISR". */
bool rb_push_isr(uint8_t byte) {
    uint32_t next = (rb_head + 1) % RB_SIZE;
    if (next == rb_tail) return false;   /* full: drop (overrun) */
    rb_buf[rb_head] = byte;
    rb_head = next;                      /* publish AFTER writing data */
    return true;
}
/* Consumer side - called from main loop. */
bool rb_pop_main(uint8_t *out) {
    if (rb_tail == rb_head) return false; /* empty */
    *out = rb_buf[rb_tail];
    rb_tail = (rb_tail + 1) % RB_SIZE;
    return true;
}

/* P7/P8: critical section to read a multi-word value the ISR updates.
 * On real HW these would disable/enable IRQs; here they are no-ops. */
static inline void irq_disable(void) { /* __disable_irq(); */ }
static inline void irq_enable(void)  { /* __enable_irq();  */ }

static volatile uint32_t g_hi = 0, g_lo = 0;   /* a 64-bit value in two words */

uint64_t read_64_atomic(void) {
    uint32_t hi, lo;
    irq_disable();                 /* critical section: ISR can't interrupt */
    hi = g_hi; lo = g_lo;
    irq_enable();
    return ((uint64_t)hi << 32) | lo;
}

/* P11: a reentrant (ISR-safe) function: no static/global state, no malloc. */
int reentrant_square(int x) { return x * x; }

int main(void) {
    /* P4: simulate three timer interrupts, main consumes the flag. */
    int handled = 0;
    for (int i = 0; i < 3; i++) {
        timer_isr();                       /* hardware would call this */
        if (g_tick_flag) { g_tick_flag = 0; handled++; }
    }
    printf("P4  ticks=%u handled=%d\n", g_tick_count, handled);

    /* P5: UART RX ISR pushes bytes, main loop drains them. */
    const char *msg = "HELLO";
    for (const char *p = msg; *p; p++) rb_push_isr((uint8_t)*p);
    printf("P5  ring buffer drain    -> ");
    uint8_t b;
    while (rb_pop_main(&b)) printf("%c", b);
    printf("\n");

    /* P6: demonstrate overrun handling (push 10 into size-8 buffer). */
    int dropped = 0;
    for (int i = 0; i < 10; i++) if (!rb_push_isr((uint8_t)i)) dropped++;
    printf("P6  overrun: dropped %d bytes (buffer full)\n", dropped);

    /* P8: atomic 64-bit read via critical section. */
    g_hi = 0x0000ABCD; g_lo = 0x12345678;
    printf("P8  read_64_atomic       = 0x%016llX\n",
           (unsigned long long)read_64_atomic());

    /* P11 */
    printf("P11 reentrant_square(9)  = %d\n", reentrant_square(9));

    printf("P3  rule: ISR sets flag/pushes data; main does the heavy work\n");
    return 0;
}
