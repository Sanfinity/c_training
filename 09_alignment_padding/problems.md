# 09 · Alignment & Padding

Why structs are bigger than the sum of their members, what "natural alignment" means, and
how to control it. Critical for register maps, DMA buffers, and wire protocols on embedded
targets. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **What is data alignment?** Why must an N-byte type sit on an N-byte boundary on many CPUs?
2. **What is padding** and why does the compiler insert it?
3. **Compute `sizeof` of a struct by hand**, accounting for padding and trailing padding.
4. **Reorder members** to minimize a struct's size.
5. **`alignof` / `_Alignof`** a type; **`alignas` / `_Alignas`** to over-align.
6. **Why is the struct's total size a multiple of its largest alignment?** (arrays of structs.)
7. **Consequences of misaligned access** — fault on ARM/some MCUs, slower on x86.

## Frequently asked
8. **`#pragma pack` / `__attribute__((packed))`** — remove padding and the trade-offs.
9. **Manually pad a struct** to match a hardware register layout.
10. **Check a pointer's alignment** with `((uintptr_t)p % align == 0)`.
11. **Align a pointer/size up** to the next multiple of a power-of-two boundary.
12. **`max_align_t`** and what `malloc` guarantees.

## Good to know / tricky
13. **Cache-line alignment** (e.g. 64 bytes) to avoid false sharing.
14. **Why reading a `uint32_t` from an odd address** can crash or be slow.
15. **Endianness vs alignment** — different concerns, often confused.
16. **`offsetof`** to verify a field lands where hardware expects.

## Rapid-fire viva questions
- What does `_Alignof(double)` typically return?
- Give a struct where reordering saves memory; state before/after size.
- What is the alignment of a `char`? Of a `struct{int;char;}`?
- How do you round a size `s` up to a multiple of 8 with bit ops?
- Why can `__attribute__((packed))` lead to unaligned-access faults?
