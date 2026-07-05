# 20 · Undefined Behavior (UB)

UB is the source of the nastiest C bugs and the most feared security holes. For a hardware
security lab this is central: you must be able to spot UB, explain why the compiler is allowed
to do anything, and know the defined alternatives. `solution.c` shows the CORRECT patterns and
labels the traps (it does not actually invoke UB). 

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Define undefined vs unspecified vs implementation-defined behavior.**
2. **Signed integer overflow is UB** — why `INT_MAX + 1` is not "wraps to INT_MIN". The defined fix (use unsigned or check before adding).
3. **Out-of-bounds array access** (read/write) is UB — the root of most exploits.
4. **Use-after-free / dangling pointer** dereference is UB.
5. **Dereferencing NULL** is UB.
6. **Reading an uninitialized variable** is UB.
7. **Shifting by >= bit width** (`x << 32` for 32-bit) is UB; shifting into the sign bit of a signed int is UB.

## Frequently asked
8. **Modifying a variable twice between sequence points**: `i = i++ + 1;`, `a[i] = i++;`.
9. **Strict aliasing violation** — type-punning through an incompatible pointer; the defined ways (`memcpy`, `union`, `char*`).
10. **Data race** (unsynchronized concurrent access) is UB.
11. **Signed left shift** vs unsigned; why bit-twiddling should use `unsigned`.
12. **Integer division by zero / INT_MIN / -1** overflow is UB.
13. **Returning a pointer to a local** and using it is UB.

## Good to know / tricky
14. **Why compilers exploit UB** — "the optimizer assumes UB never happens", so it can delete your overflow check.
15. **`-fsanitize=undefined` / `-fsanitize=address`** to catch UB at runtime.
16. **`memcpy` for type punning** to satisfy strict aliasing (compiles to nothing).
17. **Null-check-after-deref elimination** — the famous Linux kernel `tun` bug.

## Rapid-fire viva questions
- Difference between undefined, unspecified, and implementation-defined?
- Why is signed overflow UB but unsigned overflow defined?
- What is `i = i++ + 1;`? (UB.)
- What is the strict aliasing rule and how do you pun types legally?
- Why can the compiler legally remove a `if (p == NULL)` check after `*p`?
- What tools find UB? (UBSan, ASan, Valgrind.)
