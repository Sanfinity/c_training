# 35 · C Depth: Variadics, setjmp, Allocators, Generics

Beyond day-one C: the standard-library depth interviewers probe for — variadic functions, non-local jumps, custom allocators, an overlap-safe memmove, function-pointer dispatch, and C11 `_Generic` — each shown as a small standalone function in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Variadic functions (`<stdarg.h>`)** — `va_start`/`va_arg`/`va_end` walk arguments the ABI passed with no type or count metadata, so the callee MUST learn the arity from a leading COUNT (or a format string). `va_arg` with the wrong type is undefined behaviour — that missing type info is exactly why variadics are type-unsafe. → sum_ints (P1)
2. **Default argument promotions** — arguments passed through `...` are promoted: `float`→`double`, and `char`/`short`/`_Bool`→`int`. You must `va_arg` the *promoted* type (`int`, not `char`; `double`, not `float`) or you read garbage. → avg_ints (P2)
3. **Format-driven `va_arg` (a mini-printf)** — a real `printf` is variadic; the format string is the *only* thing telling it which type to pull for each `%` conversion. Get the specifier wrong (`%s` on an int) and it dereferences a bogus pointer — the root of format-string bugs. → mini_printf (P3)
4. **`setjmp`/`longjmp` (non-local jump)** — `setjmp` saves the stack/register context and returns 0; a later `longjmp(env, v)` unwinds back to it, making `setjmp` "return" `v` (a poor-man's try/catch). A local modified between the `setjmp` and the `longjmp` whose value is used afterward must be `volatile`, else it may be clobbered by the register rewind. → try_divide (P4)
5. **Arena / bump allocator** — carve allocations out of one FIXED buffer by advancing an offset; alignment rounds the offset UP (`(off + (a-1)) & ~(a-1)` for power-of-two `a`) so each block meets its type's requirement. Fast, zero fragmentation, no per-object free. → arena_alloc (P5)
6. **Bulk free / arena reset** — an arena frees EVERYTHING at once by rewinding the offset to 0 in O(1); there is no per-object bookkeeping, which is the whole reason to use one. Outstanding pointers into it become dangling. → arena_reset (P6)
7. **`memmove` vs `memcpy` (overlap)** — `memcpy` assumes the regions do NOT overlap and may copy in either direction; `memmove` must handle overlap by choosing the copy DIRECTION from pointer order — forward when `dst<src`, backward when `dst>src`. A naive forward loop corrupts an overlapping `dst>src` move. → my_memmove (P7)

## Frequently asked
8. **Function-pointer dispatch table** — replace a `switch` with an ARRAY of function pointers indexed by an opcode/enum; adding an operation becomes adding a table row while the call site is unchanged. Always range-check the index before you call through the pointer. → calc_dispatch (P8)
9. **`_Generic` (C11 type-generic selection)** — a compile-time `switch` on the *type* of an expression; a macro picks the right function per argument type with zero runtime cost. This is how `<tgmath.h>` maps `sqrt`/`sqrtf`/`sqrtl`. → GMAX (P9)
10. **`qsort` + comparator callback** — the stdlib `qsort` is generic over element type/size and delegates ordering to a `int(*)(const void*, const void*)` you supply — the classic C callback. Compare explicitly (`(a<b)-(a>b)`); a naive `a-b` can overflow. → sort_desc (P10)
11. **`offsetof` / `container_of`** — `offsetof(type, member)` yields a member's byte offset; subtracting it from a member's address recovers the enclosing struct's address (`container_of`). This is the trick behind intrusive lists (e.g. the Linux kernel). → node_from_weight (P11)

## Good to know / tricky
12. **`restrict`** — a promise that, for the lifetime of the pointer, the object it points to is reached ONLY through that pointer (no aliasing). It lets the optimizer keep values in registers and reorder loads/stores; breaking the promise is undefined behaviour. `memmove` can't use it (overlap is the point); `memcpy` can.
13. **`inline` vs `static inline` vs `extern inline`** — `inline` is a hint plus special linkage rules. In C99/C11 a plain `inline` definition provides an *inline definition* and needs a separate external definition somewhere; `static inline` gives each TU its own private copy (no external symbol, and an unused one won't trip `-Wunused-function`) — the safe default for header helpers.
14. **The comma operator** — `(a, b)` evaluates `a` (with a sequence point, discarding its value) then yields `b`. Handy in `for`-clauses (`for (i=0, j=n; ...)`); don't confuse it with the argument-separating comma in a call, which is NOT the operator.
15. **Compound literals (C99)** — `(int[]){1,2,3}` / `(struct P){.x=1}` create an anonymous, modifiable object right where written. At block scope it has automatic lifetime (dies with the block — don't return its address); at file scope it has static lifetime.
16. **`va_copy` and single-pass `va_list`** — a `va_list` is consumed once and cannot be "rewound"; to traverse the args twice, `va_copy` a backup first (and `va_end` each copy). Pass the `va_list` itself (not `...`) to forward variadic args to the `vprintf`/`vsnprintf` family.
17. **Alignment (`_Alignof` / `_Alignas`, `max_align_t`)** — every type has an alignment; a misaligned access is UB on strict ISAs and slow on others. `_Alignof(T)` queries it, `_Alignas(T)`/`_Alignas(n)` over-aligns an object, and `malloc` already returns memory aligned for any type (`max_align_t`) — but a hand-rolled allocator must align itself.
18. **`longjmp` caveats** — a `longjmp` value of 0 is silently turned into 1 (so `setjmp` never spuriously "returns 0" from a jump); non-`volatile` locals with automatic storage have indeterminate values after the jump; and jumping into a function whose activation has ALREADY RETURNED (its `jmp_buf` is stale) is undefined behaviour.

## Rapid-fire viva questions
- Why must a variadic function be told its argument count or given a format string?
- Which default argument promotions apply to `...` args, and what breaks if you `va_arg` the pre-promotion type?
- What problem does `va_copy` solve, and how do you forward `...` to `vsnprintf`?
- Which locals must be `volatile` across `setjmp`, and why exactly?
- Why is `longjmp` into a function that has already returned undefined? What happens to a `longjmp` value of 0?
- How does a bump allocator round an offset up for alignment, and why must the alignment be a power of two?
- Why does `memcpy` corrupt an overlapping copy but `memmove` doesn't? How is the direction chosen?
- What does `restrict` promise the compiler, and what is the consequence of lying?
- Contrast `inline`, `static inline`, and `extern inline` linkage in C.
- What is the value and sequencing of a comma-operator expression `(a, b)`?
- What is the lifetime of a compound literal at block scope vs file scope? Is returning its address safe?
- How does `container_of` recover a struct pointer from a member pointer, and what computes the offset?
