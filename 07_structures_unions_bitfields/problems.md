# 07 · Structures, Unions & Bit-fields

`struct`, `union`, and bit-fields are everywhere in embedded code (register maps, protocol
headers, memory-constrained data). Padding/alignment and the union "type-pun" trick are
favorite interview topics. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Define and use a struct**; access via `.` and `->`.
2. **`sizeof` a struct and explain padding** — why it isn't just the sum of members.
3. **Reorder members to minimize padding.**
4. **Union basics** — all members share storage; `sizeof(union)` = largest member.
5. **Use a union to inspect the bytes of a value** (e.g. check endianness, split a float).
6. **Bit-fields** — pack flags/fields into specific bit widths (register modeling).
7. **`typedef struct`** for cleaner types; self-referential struct (linked-list node).
8. **Pass a struct by value vs by pointer** — cost and semantics.

## Frequently asked
9. **`offsetof`** a member; why/when it's used (serialization, container_of).
10. **`container_of` macro** (Linux-kernel style) — get the struct from a member pointer.
11. **Flexible array member** (`int data[];` at the end of a struct).
12. **Anonymous unions/structs** inside a struct (C11) for register overlays.
13. **Packed struct** (`__attribute__((packed))`) for protocol/wire formats — and its risks.
14. **Model a hardware register** with a bit-field struct AND with masks/shifts; compare.

## Good to know / tricky
15. **Bit-field portability pitfalls** — allocation order and packing are implementation-defined.
16. **Struct assignment** copies member-wise (shallow) — pointer members are shared.
17. **Alignment of the whole struct** = alignment of its most-aligned member.
18. **Type punning via union vs via pointer cast** — which is defined in C? (union is OK in C.)

## Rapid-fire viva questions
- Why is `sizeof(struct{char c; int i;})` 8 and not 5?
- Difference between struct and union in one sentence?
- Can you have a bit-field of type `float`? (No.)
- What does `->` desugar to?
- How do you read/write bit 5 of a status register two different ways?
- Why can a packed struct cause an unaligned-access fault on some MCUs?
