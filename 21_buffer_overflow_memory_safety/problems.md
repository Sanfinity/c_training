# 21 · Buffer Overflows & Memory Safety

The bread-and-butter of a hardware/firmware **security** lab. You must be able to explain how
overflows corrupt memory, how classic exploits work, and — most importantly — how to write
memory-safe C. `solution.c` demonstrates the SAFE patterns, plus one **controlled overflow (P1)
that visibly corrupts an adjacent variable**; full exploits (return-address hijacking) are
described in comments only.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **What is a buffer overflow?** Stack vs heap overflow; what gets corrupted (return address, adjacent data, heap metadata). *(P1 demonstrates adjacent-variable corruption you can see.)*
2. **The classic `gets()` / `strcpy()` / `sprintf()` vulnerabilities** and their safe replacements (`fgets`, `strncpy`/`snprintf`, bounds).
3. **Off-by-one** — the `<=` vs `<` loop bug and the missing `'\0'` slot.
4. **Safe bounded copy** — write a copy that never overflows and always NUL-terminates.
5. **Integer overflow leading to under-allocation** (`malloc(n * size)` wraps → tiny buffer → overflow).
6. **Validate length/index before use** (bounds checking).
7. **Format-string vulnerability**: `printf(user)` vs `printf("%s", user)`.

## Frequently asked
8. **Stack canary / stack protector** — how it detects overflow (`-fstack-protector`).
9. **ASLR, DEP/NX, RELRO** — mitigations and what each stops.
10. **Use-after-free & double-free** as exploitable bugs (heap grooming).
11. **`strncpy` pitfalls** — doesn't NUL-terminate on truncation; write a correct wrapper.
12. **Bounds-checked array accessor.**
13. **Constant-time comparison** for secrets (avoid timing side channels).

## Good to know / tricky
14. **Heap metadata corruption** and why `free()` of a corrupted chunk is dangerous.
15. **Return-oriented programming (ROP)** — one-line concept.
16. **`memset` being optimized away** for scrubbing secrets; use `memset_s`/`explicit_bzero`.
17. **Sanitizers**: ASan for spatial/temporal safety, valgrind.

## Rapid-fire viva questions
- What memory does a stack buffer overflow corrupt? Why is the return address the target?
- Why is `gets()` removed from C11?
- Fix `char b[8]; strcpy(b, input);` safely.
- Why is `printf(user_string)` a vulnerability?
- How does a stack canary work?
- Why can `n * size` in `malloc` be dangerous, and what does `calloc` do about it?
- Why use a constant-time compare for password/HMAC checks?
