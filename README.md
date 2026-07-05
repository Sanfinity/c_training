# C / C++ Interview Practice

Topic-by-topic practice for embedded / firmware / systems-security interviews
(tailored for a Qualcomm Hardware Security Lab style loop). Each folder contains:

- **`problems.md`** — the questions to study, grouped by how often they're asked
  (*Must-know → Frequently asked → Good to know → Rapid-fire viva*).
- **`practice.c`** (or **`practice.cpp`** for C++ topics) — a **fill-in template**: every
  problem function is stubbed with a `TODO` + a hint, and `main()` is a ready-made test
  harness. **You** implement the bodies, compile, and check the output. It compiles and
  runs as-is (stubs print placeholder values) so you can work one function at a time.
- **`solution.c`** (or **`solution.cpp`**) — the **answer key**: the same file fully worked,
  so you can compare after attempting the practice file.

There is also a top-level [**`CHEATSHEET.md`**](CHEATSHEET.md) — which `#include` to use when,
compile flags, and per-topic tricks & gotchas.

## How to build & run

**Practice** (fill in `practice.c` first, then build with `-Wall`):
```
gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice          # C topics
g++ -std=c++17 -Wall -O2 practice.cpp -o practice && ./practice      # C++ topics
```
(The practice files use `-Wall` without `-Wextra` so unfilled stubs don't spam
*unused-parameter* warnings; add `-Wextra` once you've implemented them.)

**Answer key** (compiles cleanly under the strict flags):
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution          # C topics
g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o solution && ./solution      # C++ topics
```
Every `solution` compiles cleanly with `-Wall -Wextra` (no warnings).

### Suggested workflow
1. Read `problems.md` for the topic.
2. Open `practice.c` and implement each `TODO` (hints are in the comments; `main()` already calls your functions).
3. Build & run — compare your labeled output against the numbers in `solution.c`.
4. Stuck? Peek at `solution.c`, then keep the trick in mind via `CHEATSHEET.md`.

## Topics

### Core C fundamentals
| # | Topic | Focus |
|---|-------|-------|
| 01 | [Bitwise](01_bitwise/problems.md) | masks, popcount, power-of-two, rotates, XOR tricks |
| 02 | [Pointers](02_pointers/problems.md) | pointer arithmetic, `const`, function pointers, `void*` |
| 03 | [Arrays](03_arrays/problems.md) | in-place algorithms, rotations, Kadane, Dutch flag |
| 04 | [Strings](04_strings/problems.md) | implement `str*`, palindrome, anagram, atoi, balance |
| 05 | [Memory management](05_memory_management/problems.md) | malloc/free, leaks, deep copy, fixed-block pool |
| 06 | [Storage classes & qualifiers](06_storage_classes_qualifiers/problems.md) | `static`, `extern`, `volatile`, `const`, `const volatile`, const arrays |
| 07 | [Structs, unions, bit-fields](07_structures_unions_bitfields/problems.md) | padding, `container_of`, register modeling |
| 08 | [Preprocessor & macros](08_preprocessor_macros/problems.md) | `MIN/MAX`, `##`/`#`, X-macros, `do{}while(0)` |
| 09 | [Alignment & padding](09_alignment_padding/problems.md) | `alignof`/`alignas`, packing, `align_up` |
| 10 | [Endianness](10_endianness/problems.md) | detect, byte-swap 16/32/64, serialize/deserialize |
| 11 | [Data types & conversions](11_data_types_conversions/problems.md) | promotion, signed/unsigned traps, float pitfalls |

### Data structures & algorithms
| # | Topic | Focus |
|---|-------|-------|
| 12 | [Recursion](12_recursion/problems.md) | factorial, fib, fast-pow, Hanoi, permutations |
| 13 | [Linked lists](13_linked_lists/problems.md) | reverse, cycle, middle, merge, dedup |
| 14 | [Stacks & queues](14_stacks_queues/problems.md) | ring buffer, RPN, queue-from-stacks, min-stack |
| 15 | [Trees & BST](15_trees/problems.md) | traversals, height, validate, LCA, balance |
| 16 | [Sorting & searching](16_sorting_searching/problems.md) | merge/quick/insertion, binary search, quickselect |

### Low-level & RTOS
| # | Topic | Focus |
|---|-------|-------|
| 17 | [Memory-mapped I/O](17_memory_mapped_io/problems.md) | `volatile` registers, RMW, W1C, register structs |
| 18 | [Interrupts & ISRs](18_interrupts_isr/problems.md) | ISR rules, `volatile` flags, SPSC ring buffer |
| 19 | [Concurrency & RTOS](19_concurrency_rtos/problems.md) | mutex/semaphore, race, deadlock, priority inversion |

### Memory safety & undefined behavior
| # | Topic | Focus |
|---|-------|-------|
| 20 | [Undefined behavior](20_undefined_behavior/problems.md) | signed overflow, aliasing, safe patterns |
| 21 | [Buffer overflows & memory safety](21_buffer_overflow_memory_safety/problems.md) | overflow-violation demo, bounded copy, format strings, constant-time compare |

### C++ for interviews
| # | Topic | Focus |
|---|-------|-------|
| 22 | [OOP basics](22_cpp_oop_basics/problems.md) | Rule of 3/5, deep copy, operators, static members |
| 23 | [Virtual & polymorphism](23_cpp_virtual_polymorphism/problems.md) | vtables, virtual dtor, `override`, `dynamic_cast` |
| 24 | [Smart pointers & RAII](24_cpp_smart_pointers_raii/problems.md) | `unique/shared/weak_ptr`, custom deleter, cycles |
| 25 | [Templates, STL & move](25_cpp_templates_stl_move/problems.md) | templates, STL, move semantics, perfect forwarding |

### Hardware security & reverse engineering
| # | Topic | Focus |
|---|-------|-------|
| 26 | [Side-channel & fault injection](26_side_channel_fault_injection/problems.md) | constant-time compare, branchless select, glitch-hardened booleans, redundancy, DPA masking |
| 27 | [Applied cryptography](27_applied_cryptography/problems.md) | XOR/stream cipher, FNV/djb2 hash, CRC32, HMAC, LCG/xorshift PRNG, mod-exp/RSA, constant-time eq |
| 28 | [Secure boot & chain of trust](28_secure_boot_chain_of_trust/problems.md) | measured boot, hash+signature verify, anti-rollback, key revocation, PCR extend |
| 29 | [TrustZone & TEE](29_trustzone_tee/problems.md) | secure/normal world switch, SMC/HVC, secure monitor dispatch, shared-buffer checks |
| 30 | [ARM assembly & reverse engineering](30_arm_assembly_reverse_engineering/problems.md) | toy disassembler + bytecode VM, condition codes, branch classify, symbol resolve, patching |
| 31 | [Vulnerability classes & exploitation](31_vulnerability_classes_exploitation/problems.md) | use-after-free/double-free, integer-overflow checks, bounds, TOCTOU, secret scrubbing |

### Embedded firmware & advanced C/DSA
| # | Topic | Focus |
|---|-------|-------|
| 32 | [Peripheral protocols & DMA](32_peripheral_protocols_dma/problems.md) | UART/SPI/I2C bit-level, RX ring buffer, CRC-16, DMA descriptors & scatter-gather, IOMMU/SMMU |
| 33 | [Firmware building blocks](33_firmware_building_blocks/problems.md) | switch & table FSM, ring buffer, software-timer scheduler, debounce, CRC-8/16, fixed-point Q, event groups |
| 34 | [DSA: hashing, heaps, graphs, DP](34_dsa_hash_heap_graph_dp/problems.md) | hash table, min-heap/priority queue, graph BFS/DFS, DP (coin change, LIS, edit distance) |
| 35 | [C depth: variadics, setjmp, allocators, generics](35_c_depth_extras/problems.md) | variadics & mini-printf, `setjmp`/`longjmp`, arena allocator, overlap-safe `memmove`, dispatch table, `_Generic` |

### Secure design
| # | Topic | Focus |
|---|-------|-------|
| 36 | [Threat modeling & secure design](36_threat_modeling_secure_design/problems.md) | STRIDE/DREAD, fail-closed OTA state machine, anti-rollback, least-privilege caps, secret scrub, debug unlock |

## Suggested study order
1. **01, 02, 07, 06, 17, 18** — the embedded core (bits, pointers, structs, `volatile`, registers, ISRs).
2. **05, 09, 10, 11, 20, 21** — memory, alignment, endianness, conversions, and memory-safety.
3. **03, 04, 12–16, 34** — the DSA warm-ups that appear in phone screens.
4. **19, 32, 33** — RTOS/concurrency, peripherals/DMA, and firmware building blocks.
5. **26, 27, 28, 29, 30, 31, 36** — the hardware-security core this loop targets (side-channel/FI, crypto, secure boot, TrustZone, reverse engineering, vuln classes, threat modeling).
6. **08, 35** — preprocessor and deeper C (variadics, `setjmp`, allocators, generics).
7. **22–25** — C++ if the role expects it.

## Build all at once
Build & run every **answer key** from this directory (bash), cleaning up the binaries:
```bash
for d in */ ; do
  if [ -f "$d/solution.c" ]; then
    gcc -std=c11 -Wall -Wextra -O2 "$d/solution.c" -o "$d/sol" && "$d/sol"
  elif [ -f "$d/solution.cpp" ]; then
    g++ -std=c++17 -Wall -Wextra -O2 "$d/solution.cpp" -o "$d/sol" && "$d/sol"
  fi
  rm -f "$d/sol"
done
```
Swap `solution` → `practice` and drop `-Wextra` to instead build all your practice attempts.
