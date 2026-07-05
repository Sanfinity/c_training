# 18 · Interrupts & ISRs

Interrupt handling is the defining skill of embedded firmware. Interviewers probe what you
can/can't do in an ISR, `volatile` shared state, keeping ISRs short, and the ISR↔main
communication patterns (flags, ring buffers). The demo simulates an interrupt by calling the
handler directly. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **What is an ISR and how does it differ from a normal function?** (invoked by hardware, no args/return, must be fast.)
2. **Why shared variables between an ISR and main must be `volatile`** — and why that still isn't enough for multi-byte atomicity.
3. **Keep ISRs short**: set a flag / push to a buffer, defer work to main loop (top-half / bottom-half).
4. **ISR ↔ main via a `volatile` flag** — the classic pattern.
5. **ISR → main via a ring buffer** (e.g. UART RX) — SPSC lock-free.
6. **What you must NOT do in an ISR** (blocking, `malloc`, long loops, printf, taking a mutex that main holds).
7. **Enabling/disabling interrupts** and the critical-section pattern for shared data.

## Frequently asked
8. **Race condition** when main reads a multi-word value the ISR updates; fix with a critical section or double-read.
9. **Interrupt latency** — what contributes to it.
10. **Nested / prioritized interrupts** — concept (NVIC on Cortex-M).
11. **Reentrancy** — what makes a function reentrant / ISR-safe.
12. **Debouncing** a button in an ISR + timer.
13. **`sig_atomic_t`** — the one type guaranteed atomic for flags.

## Good to know / tricky
14. **Why `printf` in an ISR is dangerous** (not reentrant, slow, may block).
15. **Memory barriers** around shared-state access on multi-core / out-of-order CPUs.
16. **Losing interrupts** (overrun) if the ISR is too slow or the buffer overflows.
17. **Read-clear status registers** inside the ISR to acknowledge the interrupt.

## Rapid-fire viva questions
- Can an ISR take arguments or return a value? (No.)
- Why must the shared flag be `volatile`?
- Name three things you should never do in an ISR.
- What is a reentrant function?
- Why split work into top-half (ISR) and bottom-half (main)?
- What is `sig_atomic_t` for?
