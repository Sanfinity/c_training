# 33 · Firmware Building Blocks

The reusable primitives every embedded system is stitched from — state machines, ring buffers, software timers, debouncers, CRCs, fixed-point math, and event flags — plus the startup, linker-layout, and `volatile`/watchdog facts interviewers probe around them.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Finite-state machine (switch style)** — model behaviour as (state, event) → next-state; a `switch` on the state with per-event branches is the most readable form for a handful of states, but the code grows with every state and event you add. → fsm_step_switch (P1)
2. **Table-driven FSM** — store the machine as *data*: a `table[state][event]` of next-states, so the engine is a single lookup. Compact, auditable, and extended by editing one row instead of adding code paths. → fsm_step_table (P2)
3. **Ring buffer / circular FIFO (produce)** — a fixed array with `head`/`tail` indices and a `full` flag; the producer (often an ISR) pushes a byte or refuses when full, never overwriting unread data. The firmware workhorse for UART/DMA streams. → rb_put (P3)
4. **Ring buffer (consume, empty vs full)** — the consumer pops in FIFO order; because `head == tail` means *both* empty and full, a `full` flag (or leaving one slot unused) disambiguates the two. → rb_get (P4)
5. **Software timer / cooperative scheduler** — a table of periodic tasks, each with a reload `period` and a live `countdown`; a hardware-timer `tick()` decrements them and runs the ones that hit zero, then reloads. Gives many soft timers from one hardware timer. → sched_tick (P5)
6. **`volatile` for hardware & ISR-shared state** — tells the compiler the value may change outside normal control flow (a memory-mapped register, or a flag written by an ISR), so every access is a real load/store and none are cached in a register or optimised away. It is NOT atomicity and NOT a memory barrier.
7. **Super-loop vs RTOS** — a bare-metal `while(1)` polling loop (plus ISRs) is small and deterministic but couples timing across all work; an RTOS adds pre-emptive tasks, priorities, and blocking primitives at the cost of footprint and complexity. Choose by real-time deadlines and task independence.

## Frequently asked
8. **Button debounce** — a mechanical contact bounces for milliseconds; sample periodically and only accept a level after N consecutive equal samples (shift-register or counter), turning a noisy line into one clean edge. → debounce_update (P6)
9. **CRC-8 integrity check** — a bytewise polynomial checksum (poly 0x07, init 0x00) guarding a small stored config against bit-rot; catches all single-bit errors and most bursts far better than a plain additive sum. → crc8 (P7)
10. **CRC-16-CCITT** — the 16-bit version (poly 0x1021, init 0xFFFF, no reflection) for larger firmware images; stronger error detection, still table-free when done bitwise. Classic test vector: `"123456789"` → 0x29B1. → crc16_ccitt (P8)
11. **Vector table & reset/startup sequence** — at reset the CPU loads the initial stack pointer and the reset-handler address from the vector table (fixed address / dedicated linker section), the reset handler runs C-runtime startup (copy `.data`, zero `.bss`, run constructors), then calls `main`. The interrupt vectors follow the reset vector in the same table.
12. **Linker sections `.text/.rodata/.data/.bss`** — code and constants (`.text`, `.rodata`) live in flash; initialised writable globals (`.data`) keep their init image in flash and are *copied* to RAM at startup; zero-initialised/uninitialised globals (`.bss`) occupy no flash and are *cleared* to zero in RAM. That is exactly why `.bss` is free in the image while `.data` costs both flash and RAM.
13. **Watchdog timer** — a hardware countdown that resets the MCU unless the firmware "kicks" (refreshes) it periodically; it recovers a hung or runaway system. Service it from the main flow that proves real progress, not blindly from an ISR, or it cannot detect a stuck super-loop.

## Good to know / tricky
14. **Fixed-point Q-format (to fixed)** — with no FPU, represent reals as integers scaled by 2^n (Q16.16 = ×65536); converting *in* is a scale-and-round. Deterministic and fast where floats are too costly. → to_fixed (P9)
15. **Fixed-point (back to real)** — converting *out* divides by the same scale; keep all arithmetic in the integer domain and only convert at the edges (display, I/O). → from_fixed (P10)
16. **Fixed-point multiply** — multiplying two Qm.n values yields Q(2m).(2n); widen to 64-bit *before* the multiply, then shift right by n to renormalise back to Qm.n (and avoid the intermediate overflowing). → fx_mul (P11)
17. **RTOS event group (set)** — a word of event bits; producers OR-in flags (e.g. "RX complete", "error") to signal waiters without a separate variable and wakeup per event. → evt_set (P12)
18. **Event group wait-any / clear** — a consumer wakes when ANY requested bit is set (wait-any) or when ALL are (wait-all), then clears the bits it consumed; the classic lightweight RTOS synchronisation primitive. → evt_wait_any (P13)
19. **Why avoid `malloc` in firmware** — dynamic allocation brings non-deterministic timing, heap fragmentation that eventually fails a long-running device, and an out-of-memory path most firmware cannot recover from; prefer static objects and fixed pools sized at build time.
20. **`volatile` is not atomic** — a `volatile` 32-bit read-modify-write on an 8/16-bit MCU is still several instructions an ISR can interrupt; use a critical section (disable IRQs), an atomic type, or a lock-free single-producer/single-consumer design (the ring buffer) for shared state.

## Rapid-fire viva questions
- Why does a table-driven FSM scale better than nested `switch`es, and what is the cost?
- In a ring buffer, how do you tell "empty" from "full" when both give `head == tail`?
- List the startup steps between reset and `main` — which sections are copied, which are zeroed, and from where?
- Why does `.bss` take zero space in the flash image but `.data` does not?
- What exactly does `volatile` guarantee, and what does it NOT guarantee?
- Where must you kick the watchdog, and why not from a free-running timer ISR?
- Super-loop or RTOS — name two deciding factors.
- Why is `malloc` discouraged in long-running firmware? Name two failure modes.
- In Q16.16, why must a multiply use a 64-bit intermediate and a `>> 16`?
- How many consecutive samples does a shift-register debounce need to change state, and what sets that number?
- Single-producer/single-consumer ring buffer: why can it be lock-free while a general queue cannot?
