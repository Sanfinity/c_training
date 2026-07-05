# 02 · Pointers

Pointers are the heart of C and the #1 area interviewers probe for embedded/systems roles.
Be fluent with dereferencing, pointer arithmetic, `const` placement, function pointers,
and the classic "what does this declaration mean" puzzles. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Swap two integers via pointers** (pass-by-reference in C).
2. **Reverse an array in place** using two pointers.
3. **Implement `strlen`** using pointer arithmetic (no indexing).
4. **Walk a string with a pointer** and count vowels / characters.
5. **Difference between `const char *p`, `char *const p`, and `const char *const p`.** Read declarations right-to-left.
6. **Pointer arithmetic**: what does `p + n` add in bytes? Why does `&arr[i]` equal `arr + i`?
7. **Null / dangling / wild pointers** — define each and how they arise.
8. **Double pointer (`int **`)**: modify a caller's pointer from a function (e.g. allocate inside a function).

## Frequently asked
9. **Function pointers**: declare one, assign it, and call through it (e.g. a dispatch table / callback).
10. **Array of function pointers** to build a simple calculator or state machine.
11. **`void *` generic pointer**: write a generic `my_memcpy(void *dst, const void *src, size_t n)`.
12. **Pointer to an array vs array of pointers**: `int (*p)[10]` vs `int *p[10]`.
13. **Return a pointer from a function safely** — why returning `&local` is a bug.
14. **`qsort` with a comparator function pointer.**

## Good to know / tricky
15. **Decode complex declarations**: `int *(*fp)(int, int)`, `char (*(*x[3])())[5]`.
16. **Pointer difference**: subtracting two pointers yields element count (`ptrdiff_t`).
17. Why is `sizeof(ptr)` the same for every pointer type but `sizeof(*ptr)` differs?
18. **`restrict`** keyword — what promise does it make and why does it help the optimizer?

## Rapid-fire viva questions
- What is the value/type of `arr` when you write just `arr` (array-to-pointer decay)?
- When does array decay NOT happen? (`sizeof`, `&arr`, string literal init.)
- Difference between `p++`, `*p++`, `(*p)++`, `*++p`?
- What is a `NULL` pointer guaranteed to be? Is it always all-zero bits?
- Why can't you do arithmetic on a `void *` in standard C?
- What is the difference between passing a pointer by value and passing by reference?
