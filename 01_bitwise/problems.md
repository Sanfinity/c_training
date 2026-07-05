# 01 · Bitwise Operations & Bit Manipulation

The single most-asked embedded-C interview topic. You must be fluent with `& | ^ ~ << >>`,
masks, and the classic tricks. Solutions in `solution.c` (compile & run to verify).

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Count the number of set bits** in an integer (population count). Do it in O(#set bits) with Brian Kernighan's `n &= n-1`.
2. **Check if a number is a power of two** using a single expression.
3. **Swap two variables without a temporary** (XOR swap) — and explain the aliasing pitfall.
4. **Set, clear, toggle, and check** the k-th bit of a number.
5. **Reverse the bits** of a 32-bit unsigned integer.
6. **Find the element that appears once** when every other element appears exactly twice (XOR).
7. **Isolate / remove the lowest set bit** (`n & -n`, `n & (n-1)`).
8. **Find the position of the rightmost set bit.**
9. **Multiply / divide by a power of two** using shifts; explain arithmetic vs logical shift.
10. **Count bits to flip to convert A into B** (Hamming distance = popcount(A ^ B)).

## Frequently asked
11. **Detect whether two integers have opposite signs** without branching or multiplication.
12. **Swap the two nibbles** of a byte.
13. **Rotate bits** left and right by `d` positions (no UB when `d` is 0 or 32).
14. **Find the two elements that appear once** when all others appear twice.
15. **XOR of all integers from 1..n** in O(1).
16. **Add two integers without `+` or `-`** (bitwise full adder).

## Good to know / tricky
17. **Absolute value without branching.**
18. **Check if a number has an alternating bit pattern** (0101…).
19. Why use `unsigned` for bit twiddling? (left-shifting into/through the sign bit of a signed int is UB).
20. Explain why `1 << 31` is UB for `int` but `1u << 31` is fine.

## Rapid-fire viva questions
- What does `x & (x - 1)` do? What does `x & (-x)` do?
- Difference between logical `>>` (unsigned) and arithmetic `>>` (signed)?
- What is the result of `~0`, `~0U`, `1 << 3`, `0xFF >> 4`?
- How do you build a mask for bits `[hi:lo]`?
- How to conditionally set/clear a bit from a boolean flag without an `if`?
