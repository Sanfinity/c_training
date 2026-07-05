/*
 * Topic 18: Interrupts & ISRs -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
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
    /* TODO: implement. HINT: keep it minimal -- increment the tick counter and
       raise the pending flag so the main loop knows there is work. */
}

/* P5: ISR -> main via a lock-free single-producer/single-consumer ring buffer.
 * Producer (ISR) only writes head; consumer (main) only writes tail. */
#define RB_SIZE 8
static volatile uint8_t rb_buf[RB_SIZE];
static volatile uint32_t rb_head = 0, rb_tail = 0;

/* Producer side - called from the "UART RX ISR". */
bool rb_push_isr(uint8_t byte) {
    /* TODO: implement. HINT: compute the next head (wrapping); if it would meet
       tail the buffer is full so drop and return false; otherwise store the byte
       then publish by advancing head. */
    return false; /* placeholder */
}
/* Consumer side - called from main loop. */
bool rb_pop_main(uint8_t *out) {
    /* TODO: implement. HINT: if tail equals head it is empty; otherwise read the
       byte at tail, advance tail (wrapping), and return true. */
    return false; /* placeholder */
}

/* P7/P8: critical section to read a multi-word value the ISR updates.
 * On real HW these would disable/enable IRQs; here they are no-ops. */
static inline void irq_disable(void) { /* __disable_irq(); */ }
static inline void irq_enable(void)  { /* __enable_irq();  */ }

static volatile uint32_t g_hi = 0, g_lo = 0;   /* a 64-bit value in two words */

uint64_t read_64_atomic(void) {
    /* TODO: implement. HINT: guard the two-word read with irq_disable()/irq_enable()
       so the ISR cannot change it mid-read, then assemble the 64-bit value from the
       two 32-bit halves. */
    return 0; /* placeholder */
}

/* P11: a reentrant (ISR-safe) function: no static/global state, no malloc. */
int reentrant_square(int x) {
    /* TODO: implement. HINT: use only the parameter/locals (no static or global
       state) so it is safe to re-enter, and return x squared. */
    return 0; /* placeholder */
}

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
