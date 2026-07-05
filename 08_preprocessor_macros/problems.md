# 08 · Preprocessor & Macros

The preprocessor runs before compilation: `#include`, `#define`, conditional compilation,
and function-like macros. Embedded interviews love `MIN/MAX`, the pitfalls of macros vs
`inline`, and include guards. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Object-like vs function-like macros.**
2. **Write `MIN`, `MAX`, `SQUARE`** — and explain why you must fully parenthesize arguments.
3. **The double-evaluation pitfall**: `MAX(i++, j++)`.
4. **Include guards** (`#ifndef/#define/#endif`) vs `#pragma once`.
5. **Conditional compilation** (`#if / #ifdef / #ifndef / #elif / #else / #endif`).
6. **`#define` a constant vs `const` vs `enum`** — trade-offs.
7. **Stringize `#`** and **token-paste `##`** operators.
8. **Predefined macros**: `__FILE__`, `__LINE__`, `__func__`, `__DATE__`.

## Frequently asked
9. **Write an `ARRAY_SIZE(a)` macro** — and note when it silently breaks (pointer decay).
10. **Write an `assert`-style `CHECK(cond)` macro** using `__FILE__`/`__LINE__`.
11. **Multi-statement macro** wrapped in `do { ... } while(0)` — why that idiom?
12. **A debug-print macro** that compiles to nothing when `NDEBUG` is defined.
13. **`#error` and `#warning`** for build-time guards.
14. **Function-like macro vs `static inline`** — pros/cons (type safety, debugging, code size).

## Good to know / tricky
15. **`SET_BIT`/`CLEAR_BIT`/`READ_BIT` register macros.**
16. **X-Macros** — generate parallel tables/enums from one list.
17. **Variadic macros** (`__VA_ARGS__`).
18. **Why macros have no scope and don't respect types** — the fundamental caveat.

## Rapid-fire viva questions
- Why wrap macro parameters and the whole body in parentheses?
- Why `do { } while(0)` instead of just braces for a multi-line macro?
- What does `#x` produce? What does `a ## b` produce?
- Difference between `#define PI 3.14` and `const double PI = 3.14`?
- When does `ARRAY_SIZE(arr)` give the wrong answer?
- Does the preprocessor understand C types or scope? (No.)
