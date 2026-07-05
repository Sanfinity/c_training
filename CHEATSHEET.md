# Cheatsheet — Includes, Flags & Tricks

A quick reference for the whole library. Two parts:
1. **Which `#include` to reach for** (and the C++ equivalents).
2. **Tricks & gotchas** for each topic and for common interview scenarios.

---

## 1. Which header do I `#include`?

### C standard headers
| Header | Reach for it when you need | Key names |
|--------|----------------------------|-----------|
| `<stdio.h>` | any printing / file I/O | `printf`, `snprintf`, `fgets`, `puts`, `FILE`, `EOF` |
| `<stdlib.h>` | heap, conversions, sorting, exit | `malloc`/`calloc`/`realloc`/`free`, `atoi`/`strtol`, `qsort`/`bsearch`, `abs`, `exit`, `EXIT_SUCCESS` |
| `<string.h>` | raw memory & C-string ops | `memcpy`/`memmove`/`memset`, `strlen`/`strcpy`/`strncmp`/`strchr`/`strstr`, `strtok` |
| `<stdint.h>` | fixed-width integers | `uint8_t`, `int32_t`, `uintptr_t`, `INT32_MAX`, `SIZE_MAX` |
| `<stdbool.h>` | `bool` in C | `bool`, `true`, `false` |
| `<stddef.h>` | sizes / offsets / null | `size_t`, `ptrdiff_t`, `NULL`, `offsetof` |
| `<limits.h>` | integer ranges & bit widths | `INT_MAX`, `UINT_MAX`, `CHAR_BIT` (bits per byte = 8) |
| `<inttypes.h>` | **print** stdint types portably | `PRIu32`, `PRIx64`, `PRIdPTR` |
| `<stdalign.h>` | C11 alignment keywords | `alignas`, `alignof` |
| `<stdatomic.h>` | C11 lock-free atomics | `atomic_int`, `atomic_load`, `atomic_fetch_add`, `memory_order_*` |
| `<assert.h>` | runtime & compile-time asserts | `assert`, `static_assert` (C11) |
| `<ctype.h>` | character classification | `isdigit`, `isalpha`, `isspace`, `tolower`, `toupper` |
| `<math.h>` | floating-point math (`-lm`) | `sqrt`, `pow`, `fabs`, `floor`, `round` |
| `<float.h>` | float limits/epsilon | `FLT_EPSILON`, `DBL_MAX` |
| `<errno.h>` | error codes from libc | `errno`, `EINVAL`, `ENOMEM` |
| `<stdarg.h>` | variadic functions (topic 35) | `va_list`, `va_start`, `va_arg`, `va_end`, `va_copy` |
| `<setjmp.h>` | non-local jump / error unwind (topic 35) | `setjmp`, `longjmp`, `jmp_buf` |

### C++ headers (topics 22–25)
| Header | For | Key names |
|--------|-----|-----------|
| `<iostream>` | console I/O | `std::cout`, `std::cin`, `std::cerr`, `std::endl` |
| `<string>` | text | `std::string`, `.substr`, `.find`, `std::to_string` |
| `<vector>` / `<array>` | dynamic / fixed arrays | `std::vector`, `std::array` |
| `<map>`/`<unordered_map>` | ordered / hashed key→value | `std::map`, `std::unordered_map` |
| `<set>`/`<unordered_set>` | unique keys | `std::set`, `std::unordered_set` |
| `<algorithm>` | generic algorithms | `std::sort`, `std::find`, `std::max`, `std::swap`, `std::for_each` |
| `<numeric>` | numeric folds | `std::accumulate`, `std::iota` |
| `<memory>` | smart pointers / RAII | `std::unique_ptr`, `std::shared_ptr`, `std::make_unique`, `std::weak_ptr` |
| `<utility>` | move/forward/pairs | `std::move`, `std::forward`, `std::pair`, `std::swap` |
| `<type_traits>` | compile-time type info | `std::is_integral`, `std::enable_if`, `std::is_same` |
| `<cstdint>` | fixed-width ints in C++ | `std::uint32_t` |

**Rule of thumb:** if the compiler says *"implicit declaration of function X"* or *"unknown type name Y"*, you forgot a header — grep the tables above for `X`/`Y`.

---

## 2. Compile & debug flags

```
gcc -std=c11   -Wall -Wextra -O2  file.c   -o app       # C, strict warnings
g++ -std=c++17 -Wall -Wextra -O2  file.cpp -o app       # C++
```
| Flag | Why |
|------|-----|
| `-Wall -Wextra` | turn on the warnings that catch real bugs (unused, uninitialized, sign-compare) |
| `-Wpedantic` | flag non-standard extensions |
| `-O0 -g` | debugging (no reordering, symbols); `-O2` for realistic optimizer behavior |
| `-fsanitize=address` | ASan: catches overflows, use-after-free, leaks |
| `-fsanitize=undefined` | UBSan: catches signed overflow, bad shifts, misaligned access |
| `-fstack-protector-strong` | stack canaries (the *"stack smashing detected"* abort) |
| `-lm` | link the math library for `<math.h>` |

> The **practice.c** templates compile with just `-Wall` (not `-Wextra`) so unfilled stubs don't spam *unused-parameter* warnings. Add `-Wextra` once you've implemented them.

---

## 3. Tricks & gotchas by topic

### 01 Bitwise
- `x & (x - 1)` clears the lowest set bit → `x && !(x & (x-1))` tests **power of two**.
- `x & -x` **isolates** the lowest set bit.
- Count / find bits fast: `__builtin_popcount(x)`, `__builtin_clz` (leading zeros), `__builtin_ctz` (trailing).
- set `x |= 1u<<n`  ·  clear `x &= ~(1u<<n)`  ·  toggle `x ^= 1u<<n`  ·  test `(x>>n)&1`.
- **Always shift unsigned:** `1u << 31` is fine, `1 << 31` is UB. Shift count must be `< width`.

### 02 Pointers
- Read declarations **right-to-left**: `const int *p` = pointer to const int; `int *const p` = const pointer to int.
- Function pointer: `int (*fp)(int,int) = &add;` call as `fp(1,2)`.
- After `free(p)`, set `p = NULL` (kills double-free / use-after-free).
- An array **decays** to a pointer when passed to a function → `sizeof` inside the callee gives the pointer size, not the array size. Pass the length separately.

### 03 Arrays
- Two-pointer sweep for in-place reverse / partition / Dutch-flag.
- Kadane's for max subarray; prefix sums for range queries.
- Rotate by k = reverse(0,n-1), reverse(0,k-1), reverse(k,n-1).

### 04 Strings
- Every buffer needs **`len + 1`** for the `'\0'`.
- `strncpy` does **not** NUL-terminate on truncation → prefer `snprintf(dst, n, "%s", src)`.
- `strlen` is O(n) — don't call it in a loop condition.

### 05 Memory management
- Check `malloc`/`realloc` for `NULL`.
- `calloc(n, size)` zeroes **and** checks `n*size` overflow — safer than `malloc(n*size)`.
- `realloc` into a **temp**: `t = realloc(p,n); if(!t){/*p still valid*/} else p=t;` (else a failure leaks `p`).
- Own it once, free it once. Match every `malloc` with exactly one `free`.

### 06 Storage classes & qualifiers
- `static` local → persists across calls; `static` global/func → file-private (internal linkage).
- `volatile` → re-read every access (MMIO, ISR flag, `setjmp` locals). **Not** atomic, **not** a barrier.
- `const volatile` → read-only hardware **status** register (you can't write it, HW can change it).
- `const` array / `static const` table → lives in `.rodata`/flash, not RAM. `const int a[]` param = promise not to modify caller's array.

### 07 Structs / unions / bit-fields
- `offsetof(T, m)` and the `container_of(ptr, T, m)` pattern to get the struct from a member pointer.
- Union = overlapping storage (endianness tricks, tagged variants).
- Bit-fields model hardware registers but layout is implementation-defined — don't rely on it across compilers.

### 08 Preprocessor & macros
- Parenthesize **every argument and the whole body**: `#define SQ(x) ((x)*(x))`.
- Multi-statement macro → wrap in `do { ... } while (0)`.
- `#x` stringizes, `a##b` pastes; need a second layer (`XSTR`) to expand before stringizing.
- Beware **double evaluation**: `MAX(i++, j++)` increments twice.

### 09 Alignment & padding
- Order members **largest→smallest** to minimize padding.
- `alignof(T)` / `alignas(N)`; `_Alignas`. `#pragma pack` / `__attribute__((packed))` removes padding but can cause **unaligned access** faults on some MCUs.

### 10 Endianness
- Detect: `int x=1; *(char*)&x == 1` → little-endian.
- Network order is **big-endian**: `htons`/`htonl` on the wire.
- Byte-swap fast: `__builtin_bswap16/32/64`. Serialize byte-by-byte to be endian-independent.

### 11 Data types & conversions
- **Integer promotion:** `char`/`short` become `int` in expressions.
- **Sign traps:** `-1 > 1u` is *true* (the `-1` converts to a huge unsigned). Don't mix signed/unsigned.
- `size_t` is unsigned → `for (size_t i=n-1; i>=0; i--)` never ends. Loop `while (i-- > 0)`.

### 12–16 DSA
- Overflow-safe midpoint: `mid = lo + (hi - lo)/2`.
- Linked list slow/fast pointers → cycle detection & middle node.
- Validate a BST with **min/max bounds**, not just left<node<right locally.
- Recursion: define the base case first; watch stack depth.

### 17 Memory-mapped I/O
- Access registers through `volatile uint32_t *` (or a `volatile` register struct).
- Read-modify-write (`reg |= bit`) is **not atomic** vs. an ISR — guard it.
- **W1C** ("write-1-to-clear") status bits: write the bit back to clear it, don't RMW.

### 18 Interrupts & ISRs
- Keep ISRs **short**; do the heavy work in the main loop.
- Share state with a `volatile` flag or a **single-producer/single-consumer** ring buffer.
- 32-bit value updated in an ISR read from main → may **tear** on 8/16-bit MCUs; read atomically.

### 19 Concurrency & RTOS
- `volatile` ≠ atomic ≠ memory barrier — use `<stdatomic.h>` or a lock.
- Prevent deadlock with a **global lock ordering**.
- Priority inversion → use priority inheritance / ceiling mutexes.

### 20 Undefined behavior
- Signed overflow is UB → compute in unsigned or check *before*.
- Shift ≥ bit-width, or shifting a negative, is UB.
- **Strict aliasing:** type-pun via `memcpy` or a `union`, never a reinterpreting pointer cast.

### 21 Buffer overflows & memory safety
- Bound every copy and always NUL-terminate.
- `printf(user)` is a format-string hole → `printf("%s", user)`.
- Compare secrets in **constant time** (no early-out) to avoid timing leaks.
- Mitigations: stack canary (`-fstack-protector`), ASLR, NX/DEP, RELRO, `_FORTIFY_SOURCE`.

### 22–25 C++
- **Rule of 0/3/5**: if you write one of {destructor, copy-ctor, copy-assign}, you probably need all; prefer *Rule of 0*.
- Base class with virtual methods → **virtual destructor** (else `delete base` is UB).
- `unique_ptr` by default; `shared_ptr` for shared ownership; `weak_ptr` to break reference cycles. Prefer `make_unique`/`make_shared`.
- `std::move` only **casts** to an rvalue; the moved-from object is valid but unspecified.
- Pass big objects by `const&`; for a "sink" parameter take by value and `std::move` it in.
- Mark overrides `override`; use `= default` / `= delete` to be explicit.

### 26 Side-channel & fault injection
- **Constant-time compare:** OR all per-byte `a[i]^b[i]` diffs, branch only at the end → time depends on length, not data.
- **Branchless select:** `mask = -(cond)` (all-ones when 1, all-zeros when 0), then `(a & mask) | (b & ~mask)`.
- **Fault hardening:** store booleans as far-apart magic words (not `0`/`1`), double-check every decision, and count security steps then verify the total — a glitch skips an instruction/branch.
- SCA is **passive** (timing/power/EM/cache — SPA/DPA/CPA); FI is **active** (voltage/clock/EM/laser). Masking/blinding randomizes intermediates to defeat DPA.

### 27 Applied cryptography
- Confidentiality ≠ integrity — **encrypt *and* MAC** (encrypt-then-MAC). ECB leaks plaintext patterns; never use it.
- Never reuse a nonce/IV/keystream (two-time pad: XOR of ciphertexts leaks both plaintexts).
- **Hash** = one-way (integrity); **HMAC** = keyed (authenticity); **signature** = asymmetric (authenticity + non-repudiation).
- Verify tags in **constant time**. In mod-exp/RSA math, widen to `uint64_t` **before** multiplying to avoid overflow.

### 28 Secure boot & chain of trust
- Each stage verifies the **next** before jumping to it (ROM→BL1→BL2→OS); trust anchors in immutable ROM + a fused public-key hash.
- **Anti-rollback:** compare image version against a monotonic fuse counter and **fail closed** on downgrade.
- **Measured boot** extends a hash chain into a PCR (`PCR = H(PCR ‖ measurement)`) — attestation, distinct from verified/secure boot (which *blocks* bad images).

### 29 TrustZone & TEE
- Two worlds split by the **NS bit**; the `SMC` instruction traps to the **Secure Monitor** (EL3) to switch. Secure world can read NS memory, not the reverse.
- Shared buffers live in **normal-world** memory — never trust a pointer/length from NS: bounds-check, copy-in, and avoid TOCTOU (re-read once into secure memory).

### 30 ARM assembly & reverse engineering
- AArch64 ABI: args in `x0–x7`, return in `x0`, callee-saved `x19–x28`, `sp` 16-byte aligned, `bl` sets `lr`/`x30`.
- Flags **NZCV** from a `cmp` drive conditional branches — a `cmp`+`b.eq` is an `if`. Patch a check by NOP-ing the branch or inverting the condition.
- Recon order: `strings`/symbols → `objdump -d` / Ghidra/IDA → follow the calls to the decision point.

### 31 Vulnerability classes & exploitation
- After `free`, **NULL** the pointer (kills double-free/UAF). Check `a*b` for overflow **before** you allocate.
- Bounds-check every index; validate length **before** the copy; **scrub secrets** with a `volatile`/`memset_s` write so the compiler can't optimize it away.
- **TOCTOU:** state can change between check and use — open once and act on the handle, don't re-path.

### 32 Peripheral protocols & DMA
- UART frame = start(0) + data (**LSB first**) + parity + stop(1); SPI **mode = (CPOL,CPHA)** 0–3; I2C address byte = `(addr7 << 1) | rw`.
- ISR RX → **SPSC ring buffer**; a **power-of-two** size lets you `& (size-1)` instead of `%`.
- DMA buffers: align to a cache line and **clean/invalidate** around transfers (coherency). A rogue bus-master can read all of RAM → an **IOMMU/SMMU** confines it.

### 33 Firmware building blocks
- FSM two ways: `switch(state)` or a `table[state][event]` transition matrix.
- Startup before `main`: **`.data` copied** from flash, **`.bss` zeroed**; the **vector table** sits at the reset address. Avoid `malloc` — use static pools.
- **Fixed-point Q16.16:** multiply in 64-bit then `>> 16`. Debounce = require N stable samples. Kick the watchdog.

### 34 DSA: hashing, heaps, graphs, DP
- Hash table: index `hash % cap`; resolve by chaining or **linear probing** (tombstones on delete); rehash past load factor ~0.7.
- Array **binary heap:** parent `(i-1)/2`, children `2i+1`/`2i+2`; push sifts up, pop-min sifts down — O(log n).
- **BFS** = queue (fewest hops), **DFS** = stack/recursion. **DP** needs optimal substructure + overlapping subproblems → fill a table bottom-up.

### 35 C depth: variadics, setjmp, allocators, generics
- Variadics need a **count or format** (no built-in arg count): `va_start`/`va_arg`/`va_end`, `va_copy` before a second pass. Args promote (`char`→`int`, `float`→`double`).
- `setjmp` returns 0 when set, non-zero from `longjmp`; mark locals you modify **`volatile`**; never `longjmp` into a function that has already returned.
- **Arena/bump allocator:** round offset up to alignment, hand out `base+off`, reset = offset 0. **`memmove`** handles overlap (copy backward when `dst>src`); `memcpy` does not. `_Generic` dispatches by argument **type** at compile time.

### 36 Threat modeling & secure design
- **STRIDE** to enumerate (Spoofing, Tampering, Repudiation, Info-disclosure, DoS, Elevation); **DREAD/CVSS** to rate.
- Principles: **least privilege**, **defense in depth**, **secure defaults / fail-closed**, **complete mediation**, minimize attack surface & trust boundaries.
- "Design a secure X": anchor a **root of trust** → authenticate + integrity-check every input → anti-rollback → least-privilege capabilities → scrub secrets → gate debug/JTAG behind an authenticated token.

---

## 4. Idioms worth memorizing
```c
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))
#define MAX(a,b)        (((a) > (b)) ? (a) : (b))
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* safe multi-statement macro */
#define LOG(msg)  do { fprintf(stderr, "%s\n", (msg)); } while (0)

/* overflow-safe binary-search midpoint */
int mid = lo + (hi - lo) / 2;

/* set / clear / test bit n */
reg |=  (1u << n);
reg &= ~(1u << n);
int bit = (reg >> n) & 1u;

/* constant-time equality (no early-out) — for MACs / PINs / tags / hashes */
uint8_t diff = 0;
for (size_t i = 0; i < n; i++) diff |= a[i] ^ b[i];
int equal = (diff == 0);          /* time depends on n, not on the data */

/* branchless conditional select: mask = all-ones if cond else all-zeros */
uint32_t mask = (uint32_t)-(int32_t)(cond != 0);
uint32_t out  = (a & mask) | (b & ~mask);
```
