# 30 · ARM Assembly & Reverse Engineering

Reading AArch64 disassembly, understanding the ABI/stack, and pulling a binary apart with objdump/Ghidra — the day-to-day of a hardware-security firmware review.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

> We can't execute ARM assembly on this x86-64 host, so `solution.c` **models** each
> concept in portable C and **annotates** it with the equivalent AArch64 in comments
> (a tiny bytecode disassembler + stack-machine VM is the reverse-engineering centrepiece).

---

## Must-know (asked constantly)

1. **AArch64 register file.** 31 general-purpose registers `x0`–`x30` (64-bit); each has a
   32-bit view `w0`–`w30` (writing a `w` register **zeroes** the top 32 bits). Special roles:
   `x30` = **LR** (link register, return address), `x29` = **FP** (frame pointer),
   `sp` = stack pointer (not `x31` — encoding 31 usually means `sp` or the zero register
   `xzr`/`wzr`), and `pc` is not directly addressable. `xzr` reads as 0, discards writes.

2. **Procedure Call Standard (AAPCS64).** First 8 integer/pointer args in `x0`–`x7`
   (`w0`–`w7` for 32-bit); anything more is **passed on the stack**. Integer return in
   `x0` (`x1:x0` for 128-bit). `x8` = indirect-result register (returning a big struct).
   `x9`–`x15` = **caller-saved** temporaries; `x16`/`x17` (IP0/IP1) = veneer/PLT scratch;
   `x18` = platform-reserved; `x19`–`x28` = **callee-saved** (a function must preserve them);
   `x29`/`x30` saved by the prologue. `sp` must be **16-byte aligned** at any public boundary.

3. **Stack frame — prologue & epilogue.** A non-leaf function saves FP/LR on entry and
   restores them on exit:
   ```
   func:
       stp  x29, x30, [sp, #-16]!    ; PROLOGUE: push FP,LR, pre-decrement sp (16B aligned)
       mov  x29, sp                  ; establish frame pointer
       ...                           ; body; locals live below x29
       ldp  x29, x30, [sp], #16      ; EPILOGUE: pop FP,LR, post-increment sp
       ret                           ; branch to x30 (LR)
   ```
   A **leaf** function that calls nothing and needs no stack can skip this and just `ret`.

4. **Load/store architecture.** ARM is **RISC**: *only* `LDR`/`STR` (and `LDP`/`STP`) touch
   memory; every arithmetic/logic op works **register→register**. So "read, modify, write"
   is at least three instructions — you never see `add [mem], #1` like on x86.

5. **Condition flags NZCV.** In `PSTATE`: **N** (negative), **Z** (zero), **C** (carry /
   unsigned borrow), **V** (signed overflow). Set by the `S`-form ops (`ADDS`, `SUBS`) and by
   `CMP` (which is `SUBS xzr, a, b`) / `TST` (`ANDS xzr`). A following `B.cond` reads them.

6. **Core instruction vocabulary.** `MOV` (reg/imm), `ADD`/`SUB`, `CMP` (compare = flag-setting
   subtract), `B` (unconditional branch), `B.cond` (conditional), `BL` (branch-and-link, sets
   LR — a call), `BLR`/`BR` (indirect call/jump via register), `RET` (return via LR),
   `LDR`/`STR` (load/store), `LDP`/`STP` (load/store **pair** — used for prologues), plus
   `CBZ`/`CBNZ` and `TBZ`/`TBNZ` (compare/test-bit-and-branch).

---

## Frequently asked

7. **A32/T32 vs A64.** *A64* is the 64-bit ISA (AArch64): **fixed 4-byte** instructions,
   31 GP registers, no conditional execution except `B.cond`/`CSEL`. *A32* is classic 32-bit
   ARM (fixed 4-byte, predicated instructions, 16 registers). *T32* (Thumb-2) mixes 2- and
   4-byte encodings for code density. A CPU switches between A32/T32 via the `T` bit; AArch64
   and AArch32 are separate execution states.

8. **Endianness.** ARM is normally **little-endian** for data (config bit `SCTLR.EE`; `BE8`
   exists but is rare). A64 *instructions* are stored little-endian in memory, so the word
   `52 80 05 40` on disk decodes as the 32-bit encoding `0x40058052`. Multi-byte immediates
   and pointers you pull from a dump are little-endian — byte-reverse before interpreting.

9. **Spotting control flow in disassembly.** A **backward** branch (target address ≤ the
   branch's own address) closes a **loop**. A **forward conditional** branch skips a block —
   an **if**. A **switch** usually compiles to a **jump table**: `adrp`+`add` to the table
   base, `ldr`/`ldrsw` an entry indexed by the selector, then `br` to it (an indirect branch).
   Recognising these three shapes lets you read raw code before any decompiler runs.

10. **RE workflow & tools.** *Static:* `file` (identify), `strings` (embedded text/keys),
    `readelf -h/-S/-s` (ELF header, sections, symbols), `nm` (symbol table), `objdump -d`
    (disassemble), then a decompiler — **Ghidra**, **IDA Pro**, **radare2**/rizin, or Binary
    Ninja — for cross-references and pseudo-C. *Dynamic:* `gdb` (or gdb-multiarch + QEMU for
    cross-arch), `ltrace`/`strace`. Typical loop: triage → find `main`/entry → follow strings
    and calls to the interesting routine → confirm by emulating or debugging.

---

## Good to know / tricky

11. **PIC, ASLR, GOT & PLT.** Position-independent code addresses data/functions relative to
    the PC (`adrp` gives the 4 KB page base, `add`/`ldr :lo12:` the offset), so the loader can
    map it anywhere — enabling **ASLR** (randomised base) with PIE binaries. Calls to shared
    libraries go through the **PLT** (a stub) which reads the function's runtime address from
    the **GOT**; the first call resolves lazily via the dynamic linker, later calls jump straight.
    Full **RELRO** makes the GOT read-only after resolution to stop GOT-overwrite attacks.

12. **Stripped binaries & symbols.** `strip` removes `.symtab`/`.strtab`, so there are no
    function names — decompilers synthesise `sub_401000` / `FUN_00401000` / `loc_...` from the
    address. You recover meaning from strings, imports (PLT names survive as relocations),
    constants (crypto S-boxes, magic numbers), and cross-references, then rename by hand.

13. **Patching.** Reverse-engineers change behaviour by overwriting bytes: **NOP-out** a check,
    **invert** a branch (`B.EQ` ↔ `B.NE`), or force a return value (`MOV w0, #1; RET`). Because
    A64 is fixed-width you can swap one 4-byte instruction for another in place; on Thumb you
    must respect 2/4-byte boundaries. Keep the file the same size and fix any checksum/signature.

14. **CMP/branch idioms & leaf calls.** `CMP w0, #0` then `B.GT` is a signed compare; use
    `B.HI`/`B.LO` for unsigned. `CBZ x0, label` = "branch if zero" without touching flags.
    A leaf function keeps values in caller-saved `x9`–`x15` and avoids a stack frame entirely,
    which is why simple helpers disassemble to just a few instructions ending in `RET`.

---

## Rapid-fire viva questions

- Which register is the link register, and which is the frame pointer? → `x30` (LR), `x29` (FP).
- Where do the first four arguments to a function go? → `x0`–`x3` (up to `x0`–`x7`).
- Where is an `int` return value? → `w0` / `x0`.
- What does `CMP` actually do? → a `SUBS` that discards the result but sets **NZCV**.
- What are the four condition flags? → **N**egative, **Z**ero, **C**arry, o**V**erflow.
- Difference between `B` and `BL`? → `BL` also writes the return address into `x30` (it's a call).
- How does a function return? → `RET` branches to the address in `x30` (LR).
- Why is `STP x29, x30, [sp, #-16]!` the first instruction so often? → prologue: save FP+LR, 16-byte-align.
- Which registers must a callee preserve? → `x19`–`x28` (plus FP/LR/SP).
- Load/store architecture means what? → only `LDR`/`STR` touch memory; ALU ops are register-only.
- A64 instruction size? → fixed **4 bytes**; Thumb is 2 **or** 4.
- Default ARM endianness? → little-endian.
- A backward branch in a listing usually indicates…? → a **loop**.
- How does a `switch` often compile? → a **jump table** (`adrp`/`ldr`/`br`).
- What does `strip` remove and what's the effect? → `.symtab`/`.strtab`; names become `sub_XXXX`.
- Name three static-analysis tools. → `objdump`, `readelf`/`nm`, Ghidra/IDA/radare2.
- What are the GOT and PLT for? → dynamic-symbol address table (GOT) reached via call stubs (PLT).
- How would you defeat a license check in a binary? → NOP the call / invert the branch / patch the return.
- What is `xzr`/`wzr`? → the zero register: reads 0, writes are discarded.
- Why can't you `add` directly to a memory operand on ARM? → RISC load/store: compute in registers, `STR` back.
