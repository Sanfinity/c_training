# 19 · Concurrency & RTOS Concepts

Multitasking firmware runs on an RTOS (FreeRTOS, QNX, ThreadX…). Interviewers test the
mental model: tasks, scheduling, mutexes vs semaphores, race conditions, deadlock, and
priority inversion. The `solution.c` simulates the primitives single-threaded so it compiles
with the standard command; the concepts are exactly what you explain verbally.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Process vs thread vs RTOS task** — address space, stack, what's shared.
2. **What is a race condition?** Show the non-atomic `count++` (load-modify-store) problem.
3. **Mutex vs binary semaphore** — ownership, use for mutual exclusion vs signaling.
4. **Counting semaphore** — model a pool of N resources.
5. **Critical section** — protect shared data (disable IRQs or take a mutex).
6. **Deadlock** — the four Coffman conditions; show two tasks locking A/B in opposite order.
7. **Avoid deadlock with lock ordering** (always acquire in a global order).

## Frequently asked
8. **Priority inversion** — what it is and how **priority inheritance** fixes it.
9. **Producer/consumer** with a bounded buffer + semaphores (empty/full counts).
10. **Preemptive vs cooperative scheduling.**
11. **Context switch** — what the scheduler saves/restores.
12. **Spinlock vs mutex** — busy-wait vs block; when each is appropriate.
13. **Reentrancy & thread-safety** — what makes a function safe to call concurrently.

## Good to know / tricky
14. **Semaphore give from ISR** (`xSemaphoreGiveFromISR`) — signal a task from an interrupt.
15. **Starvation & livelock** vs deadlock.
16. **Memory barriers / `volatile` vs atomics** — why `volatile` isn't thread-safe.
17. **Watchdog timer** — why RTOS systems kick it from a low-priority task.

## Rapid-fire viva questions
- Mutex vs semaphore in one sentence?
- What are the four conditions for deadlock?
- How does priority inheritance solve priority inversion?
- Why is `count++` not atomic?
- Spinlock vs blocking mutex — when do you use a spinlock?
- Is `volatile` enough for thread safety? (No — need atomics/locks/barriers.)
- What does a context switch save?
