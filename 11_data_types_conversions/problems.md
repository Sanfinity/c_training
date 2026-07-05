# 11 · Data Types, Integer Promotion & Conversions

The rules that cause the most subtle bugs in C: signed vs unsigned comparisons, integer
promotion, overflow, and floating-point pitfalls. Interviewers use these to separate people
who "know C" from people who really know C. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Sizes and ranges** of `char/short/int/long/long long` and their `unsigned` forms; why to prefer `<stdint.h>` (`int32_t`, `uint8_t`…).
2. **Signed vs unsigned comparison trap**: `-1 < 1u` is *false*. Explain why.
3. **Integer promotion**: `char + char` is computed as `int`.
4. **Usual arithmetic conversions** — how a mixed `int`/`unsigned` expression is typed.
5. **Integer overflow**: signed overflow is UB, unsigned wraps modulo 2^N.
6. **Truncation** when assigning a wider type to a narrower one.
7. **Implicit vs explicit (cast) conversion**; when a cast is required.

## Frequently asked
8. **`char` signedness is implementation-defined** — why `char`, `signed char`, `unsigned char` are three distinct types.
9. **Float → int conversion** truncates toward zero; int → float may lose precision.
10. **Why `0.1 + 0.2 != 0.3`** in floating point; comparing floats with an epsilon.
11. **`sizeof` returns `size_t`** (unsigned) — the classic `for (size_t i = n-1; i >= 0; i--)` infinite loop.
12. **Bit width of the result of shifts/`~`** after promotion.
13. **Hex/octal/binary literals and suffixes** (`U`, `L`, `UL`, `LL`).

## Good to know / tricky
14. **`INT_MIN` negation and `abs(INT_MIN)`** overflow.
15. **Casting a negative int to unsigned** — the modulo-2^N result.
16. **`bool`/`_Bool`** normalizes any nonzero to 1.
17. **`(int)(float)large_value`** and undefined behavior when out of range.

## Rapid-fire viva questions
- Why is `if (-1 < sizeof(x))` almost always false?
- What type does `'A' + 1` have?
- Result of `(unsigned char)257`? Of `(char)-1` printed as unsigned?
- What is the value of `5u - 10u`?
- Is `int i = 3.9;` legal, and what is `i`?
- Why avoid `float` equality comparisons?
