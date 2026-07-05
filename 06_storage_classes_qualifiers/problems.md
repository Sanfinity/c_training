# 06 · Storage Classes & Type Qualifiers

`auto`, `register`, `static`, `extern`, plus the qualifiers `const`, `volatile`, `restrict`.
The `static` and `volatile` questions are almost guaranteed in an embedded interview. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **The four storage classes** (`auto`, `register`, `static`, `extern`) — scope, lifetime, linkage, default value.
2. **`static` local variable** — retains value between calls (write a call counter).
3. **`static` global / function** — internal linkage (file-private). Why use it?
4. **`extern`** — declaring a variable defined in another translation unit.
5. **`volatile`** — what it means, and the three classic use cases: memory-mapped registers, variables modified by an ISR, variables modified by another thread.
6. **`const`** — const variables, const pointers, const function parameters, **const arrays / read-only lookup tables**.
7. **`const volatile` together** — when is that meaningful? (read-only hardware status register.)
8. **Difference between definition and declaration.**

## Frequently asked
9. **Where is each variable stored?** (`.data`, `.bss`, stack, heap, `.rodata`, registers.)
10. **Lifetime & scope** of local vs global vs static local.
11. **What does `register` really do today?** (hint / you can't take its address.)
12. **Why can't two files both *define* the same global?** (multiple definition / linker error.)
13. **A `static` function can't be called from another file** — demonstrate the intent.

## Good to know / tricky
14. **Why `volatile` does NOT make a variable thread-safe / atomic.**
15. **Optimizer + `volatile`**: show a polling loop the compiler would break without `volatile`.
16. **`const` cast-away** and the UB of modifying a truly-const object.
17. **Default initialization**: globals/statics are zero-initialized; locals are indeterminate.
18. **`const` arrays & read-only tables** — `static const` lookup tables live in `.rodata`/flash (not RAM), the standard way to store constant tables on an MCU. A `const int a[]` parameter promises the callee won't modify the caller's array (identical to `const int *a`).

## Rapid-fire viva questions
- What is the default storage class of a local variable? A global?
- Difference between `static` at file scope and at block scope?
- Can a `const` variable be used as an array size in C? (Not for a true constant expression pre-C99.)
- Give the exact scenario where forgetting `volatile` causes an infinite loop.
- Is `volatile` a substitute for a memory barrier / atomic? (No.)
