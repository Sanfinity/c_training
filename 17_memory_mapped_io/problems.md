# 17 · Memory-Mapped I/O & Register Access

The core skill of bare-metal firmware: talking to hardware through registers at fixed
addresses. Interviewers check that you use `volatile`, correct pointer casts, and safe
read-modify-write. The demo simulates registers with `volatile` variables so it runs on a PC.
Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Access a register at a fixed address**: `#define REG (*(volatile uint32_t *)0x40021000)`. Why `volatile`? Why `uint32_t`?
2. **Read-modify-write a register field** without disturbing other bits.
3. **Set / clear / toggle a specific bit** in a hardware register.
4. **Poll a status/ready bit** in a loop (why `volatile` is mandatory here).
5. **Write a bit-field of value into a masked field** (shift + mask).
6. **Map a register block with a `struct`** of `volatile` members at a base address.
7. **Why you must not let the compiler reorder or cache** device accesses.

## Frequently asked
8. **Write-1-to-clear (W1C)** status bits — a common hardware idiom.
9. **Separate SET and CLEAR registers** (atomic bit ops without RMW, e.g. GPIO BSRR).
10. **`volatile` placement**: `volatile uint32_t *` vs `uint32_t * volatile`.
11. **Memory barriers / `__DMB()`** — why ordering matters around device access.
12. **Reading a 64-bit counter split across two 32-bit registers** safely (high/low re-read).

## Good to know / tricky
13. **Padding/reserved fields** in a register-map struct (use `_rsvd` arrays).
14. **`const volatile`** for read-only status registers.
15. **Why `int`-sized access to an 8-bit register** can be wrong (access width matters).
16. **Bit-banding** (Cortex-M) concept.

## Rapid-fire viva questions
- What does `volatile` guarantee and NOT guarantee?
- Why cast to `uint32_t *` and not `int *`?
- Show the safe way to set bits [5:4] to `0b10` in a 32-bit register.
- What is write-1-to-clear and why does hardware use it?
- Why can polling a non-`volatile` flag hang forever?
- Does `volatile` provide multi-core ordering? (No — need barriers.)
