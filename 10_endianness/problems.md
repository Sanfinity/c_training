# 10 · Endianness

Byte order matters whenever data crosses a boundary: networking, file formats, inter-processor
comms, and reading multi-byte hardware registers. A staple embedded/systems interview topic.
Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Define big-endian vs little-endian.** Which stores the least-significant byte first?
2. **Detect the machine's endianness at runtime** — with a union and with a pointer cast.
3. **Swap the byte order of a 16-bit value** (`bswap16`).
4. **Swap the byte order of a 32-bit value** (`bswap32`).
5. **Swap a 64-bit value** (`bswap64`).
6. **Why network byte order is big-endian** and what `htons/htonl/ntohs/ntohl` do.
7. **Read a 32-bit value from a byte buffer** in a chosen endianness (portable deserialization).

## Frequently asked
8. **Write a 32-bit value into a byte buffer** in big- and little-endian (serialization).
9. **Convert host→network and network→host** for a struct/packet field.
10. **What breaks when two machines of different endianness exchange raw structs?**
11. **Endianness and unions** — why a union type-pun reveals byte order.
12. **Bit-order vs byte-order** — are they the same thing? (No.)

## Good to know / tricky
13. **Middle-endian / mixed-endian** (historical, rare).
14. **Does endianness affect a single `char`/`uint8_t`?** (No.)
15. **Endianness of bit-fields** — implementation-defined, a real portability trap.
16. **Compiler builtins** `__builtin_bswap32`, `<endian.h>` helpers.

## Rapid-fire viva questions
- On a little-endian machine, how is `0x12345678` stored in memory (byte by byte)?
- How do you detect endianness in a single expression?
- What is host-to-network for a 16-bit port number?
- Why doesn't endianness matter for a byte array like a C string?
- If you `memcpy` a struct over a socket between an x86 and a big-endian MCU, what happens?
