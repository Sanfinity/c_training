# 05 · Memory Management (heap, malloc/free)

Dynamic memory is where embedded/systems interviews get serious: leaks, double-frees,
fragmentation, and knowing exactly what `malloc`/`calloc`/`realloc`/`free` do. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **`malloc` vs `calloc` vs `realloc` vs `free`** — signatures, what each does, zero-init behavior.
2. **Allocate, use, and free a dynamic array** correctly; always check for `NULL`.
3. **Allocate a dynamic 2-D array** (array of pointers, and single-block variants).
4. **Grow an array with `realloc`** — the correct idiom (don't overwrite the original pointer directly).
5. **What is a memory leak?** Show one and its fix.
6. **Dangling pointer / use-after-free / double-free** — define and demonstrate the danger.
7. **Deep copy vs shallow copy** of a struct containing a pointer.
8. **`free` doesn't null the pointer** — why you should set it to `NULL` after freeing.

## Frequently asked
9. **Stack vs heap** — lifetime, size, speed, who manages it.
10. **Implement a simple fixed-block memory pool / allocator** (common embedded question — no `malloc`).
11. **Where does `malloc` get memory from?** (`sbrk`/`mmap`, the C runtime heap.)
12. **What does `malloc(0)` return?** (Implementation-defined: NULL or a freeable unique pointer.)
13. **Alignment guarantees of `malloc`** (suitably aligned for any object).
14. **Detecting/avoiding fragmentation** on long-running embedded systems.

## Good to know / tricky
15. **`realloc` with size 0** and `realloc(NULL, n)` semantics.
16. **Why casting `malloc`'s return in C is discouraged** (but required in C++).
17. **Memory-mapped vs dynamically-allocated** memory.
18. **RAII-style cleanup in C** using `goto cleanup;` patterns.

## Rapid-fire viva questions
- What happens if you `free` the same pointer twice?
- What happens if you `free` a pointer not returned by `malloc`?
- Does `free` know the size of the block? How?
- After `free(p)`, is `p` safe to read/compare? (No — it's indeterminate.)
- Difference between `calloc(n, size)` and `malloc(n*size)` beyond zeroing? (overflow-checked multiply.)
- What tools find leaks (Valgrind, ASan)?
