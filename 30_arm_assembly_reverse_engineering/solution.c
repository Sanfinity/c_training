/*
 * Topic 30: ARM Assembly & Reverse Engineering  (solution / answer key)
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * We cannot run AArch64 assembly on this x86-64 MinGW host, so every concept is
 * MODELLED in portable C11 and ANNOTATED with the equivalent AArch64 in comments.
 * Each Pn function is standalone (its only dependencies are the const opcode
 * tables / helpers below), so you can lift one into your own harness. The tiny
 * bytecode disassembler (P4) + stack-machine VM (P5) are the RE centrepiece:
 * "here are some bytes -> what instructions are they -> what do they compute?".
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>

/* ------------------------------------------------------------------------- *
 *  Scaffolding: a tiny bytecode ISA shared by the disassembler + the VM.
 *  (Kept intact in practice.c — you implement the Pn functions, not these.)
 * ------------------------------------------------------------------------- */
enum {
    OP_HALT  = 0x00,   /* stop the machine                        */
    OP_PUSH  = 0x01,   /* PUSH imm32   (4 little-endian operand bytes follow) */
    OP_ADD   = 0x02,   /* pop b, pop a, push (a + b)              */
    OP_MUL   = 0x03,   /* pop b, pop a, push (a * b)              */
    OP_PRINT = 0x04,   /* print top of stack (peek, no pop)       */
    OP_NOP   = 0x05    /* do nothing (handy as a patch/pad byte)  */
};

/* Sample program:  (3 + 4) * 6  ->  42   (it PRINTs 42 and returns 42). */
static const uint8_t SAMPLE_PROG[] = {
    OP_PUSH, 0x03, 0x00, 0x00, 0x00,   /* off 0 : PUSH 3      */
    OP_PUSH, 0x04, 0x00, 0x00, 0x00,   /* off 5 : PUSH 4      */
    OP_ADD,                            /* off 10: ADD  -> 7   */
    OP_PUSH, 0x06, 0x00, 0x00, 0x00,   /* off 11: PUSH 6      */
    OP_MUL,                            /* off 16: MUL  -> 42  */
    OP_PRINT,                          /* off 17: PRINT 42    */
    OP_HALT                            /* off 18: HALT        */
};
#define MUL_OFFSET 16   /* byte offset of the MUL opcode (patched in P10) */

/* Mnemonic table for the disassembler (scaffolding). External linkage so the
 * practice template stays warning-clean even when disassemble() is still a stub. */
const char *opcode_name(uint8_t op) {
    switch (op) {
        case OP_HALT:  return "HALT";
        case OP_PUSH:  return "PUSH";
        case OP_ADD:   return "ADD";
        case OP_MUL:   return "MUL";
        case OP_PRINT: return "PRINT";
        case OP_NOP:   return "NOP";
        default:       return ".byte";
    }
}

/* AArch64 condition-code mnemonics 0x0..0xF (scaffolding for P7). */
static const char *cond_name(uint8_t cond) {
    static const char *const n[16] = {
        "EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC",
        "HI", "LS", "GE", "LT", "GT", "LE", "AL", "NV"
    };
    return n[cond & 0xF];
}

/* Helper (scaffolding, kept intact in practice.c): xxd-style hex + ASCII dump,
 * used to "inspect" the raw bytecode the way you'd eyeball a firmware image. */
static void hexdump(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i += 16) {
        printf("    %04zx: ", i);
        for (size_t j = 0; j < 16; j++) {
            if (i + j < len) printf("%02x ", data[i + j]);
            else             printf("   ");
        }
        printf(" |");
        for (size_t j = 0; j < 16 && i + j < len; j++) {
            uint8_t c = data[i + j];
            putchar((c >= 0x20 && c < 0x7f) ? (int)c : '.');
        }
        puts("|");
    }
}

/* ------------------------------------------------------------------------- *
 *  P1: integer add — the canonical "what does this compile to?" question.
 *  AArch64:
 *      add:  ADD  w0, w0, w1     ; w0 = arg0 + arg1  (32-bit -> use w regs)
 *            RET                 ; result stays in w0; RET branches to x30/LR
 *  AAPCS64: arg0 in w0/x0, arg1 in w1/x1, return value in w0/x0.
 * ------------------------------------------------------------------------- */
static int add(int a, int b) {
    return a + b;
}

/* ------------------------------------------------------------------------- *
 *  P2: recursive sum 1..n — shows a NON-LEAF prologue/epilogue + stack use.
 *  AArch64 (must preserve LR across the recursive BL, hence a stack frame):
 *      sum_to_n:
 *          cmp   w0, #0
 *          b.gt  .Lrec
 *          mov   w0, #0
 *          ret                          ; base case: leaf path, no frame
 *      .Lrec:
 *          stp   x29, x30, [sp, #-16]!  ; PROLOGUE: save FP,LR (16B-aligned push)
 *          mov   x29, sp                ; set frame pointer
 *          str   w0, [x29, #-4]         ; spill n into this frame
 *          sub   w0, w0, #1
 *          bl    sum_to_n               ; recurse; BL overwrites x30 -> saved above
 *          ldr   w1, [x29, #-4]
 *          add   w0, w0, w1
 *          ldp   x29, x30, [sp], #16    ; EPILOGUE: restore FP,LR
 *          ret                          ; branch to restored x30
 *  Each recursion level owns a frame holding its own LR + n -> that's the stack.
 * ------------------------------------------------------------------------- */
static int sum_to_n(int n) {
    if (n <= 0) return 0;
    return n + sum_to_n(n - 1);
}

/* ------------------------------------------------------------------------- *
 *  P3: decode a little-endian 32-bit immediate from a byte stream.
 *  AArch64 is little-endian, so the in-memory bytes 78 56 34 12 form 0x12345678.
 *      LDR  w0, [x1]     ; on an LE core this single load does exactly this.
 *  Used by the disassembler + VM to read PUSH operands.
 * ------------------------------------------------------------------------- */
static uint32_t read_u32_le(const uint8_t *p) {
    return  (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

/* ------------------------------------------------------------------------- *
 *  P4: linear-sweep disassembler — bytes -> mnemonics (the RE "objdump -d").
 *  Walk the stream: read an opcode, consume its operands, advance the PC.
 *  Unknown bytes are shown as ".byte" (data-in-code), exactly like a real tool.
 * ------------------------------------------------------------------------- */
static void disassemble(const uint8_t *code, size_t len) {
    size_t pc = 0;
    while (pc < len) {
        uint8_t op = code[pc];
        printf("    %04zx: %02x  %-5s", pc, op, opcode_name(op));
        if (op == OP_PUSH) {
            if (pc + 5 <= len) {
                printf(" #%" PRIu32, read_u32_le(&code[pc + 1]));
                pc += 5;
            } else {
                printf(" <truncated operand>");
                pc = len;
            }
        } else {
            pc += 1;
        }
        putchar('\n');
    }
}

/* ------------------------------------------------------------------------- *
 *  P5: execute the bytecode on a small stack machine (the RE "emulate it").
 *  A fetch/decode/execute loop with a CMP-style switch is exactly what a CPU
 *  (or an emulator like QEMU) does. Bounds are checked because a disassembler
 *  / VM parses UNTRUSTED input — malformed bytecode must not run off the ends.
 *  Returns the final top-of-stack (0 if empty).
 * ------------------------------------------------------------------------- */
static int64_t vm_execute(const uint8_t *code, size_t len, bool trace) {
    int64_t stack[64] = {0};
    size_t  sp = 0;    /* stack pointer (grows up)  */
    size_t  pc = 0;    /* program counter           */
    while (pc < len) {
        uint8_t op = code[pc++];
        switch (op) {
            case OP_HALT:
                goto done;
            case OP_PUSH:
                if (pc + 4 > len || sp >= 64) goto done;   /* malformed / overflow */
                stack[sp++] = (int64_t)read_u32_le(&code[pc]);
                pc += 4;
                break;
            case OP_ADD:
                if (sp < 2) goto done;
                { int64_t b = stack[--sp], a = stack[--sp]; stack[sp++] = a + b; }
                break;
            case OP_MUL:
                if (sp < 2) goto done;
                { int64_t b = stack[--sp], a = stack[--sp]; stack[sp++] = a * b; }
                break;
            case OP_PRINT:
                if (sp > 0 && trace) printf("    [PRINT] %" PRId64 "\n", stack[sp - 1]);
                break;
            case OP_NOP:
                break;
            default:
                /* a real tool would flag an unknown opcode as data-in-code */
                break;
        }
    }
done:
    return (sp > 0) ? stack[sp - 1] : 0;
}

/* ------------------------------------------------------------------------- *
 *  P6: dispatch through a function-pointer table — how a compiler lowers a
 *  dense `switch` into a JUMP TABLE (array of code addresses, indexed + BR).
 *
 *  Leaf ALU ops (scaffolding): their ADDRESSES populate the table below.
 * ------------------------------------------------------------------------- */
static long op_add(long a, long b) { return a + b; }
static long op_sub(long a, long b) { return a - b; }
static long op_mul(long a, long b) { return a * b; }
static long op_and(long a, long b) { return a & b; }

enum { ALU_ADD, ALU_SUB, ALU_MUL, ALU_AND, ALU_COUNT };

/* The jump table itself (external linkage keeps an unused build warning-free). */
long (*const ALU[ALU_COUNT])(long, long) = { op_add, op_sub, op_mul, op_and };

/* AArch64 lowering of `switch (op) { ... }`:
 *      adrp x1, .Ltable            ; page base of the table (PIC-friendly)
 *      add  x1, x1, :lo12:.Ltable
 *      ldr  x2, [x1, w0, uxtw #3]  ; x2 = table[op]   (8-byte entries)
 *      br   x2                     ; indirect branch to the chosen case
 * The default/out-of-range case is the bounds check.
 */
static long alu_dispatch(int op, long a, long b) {
    if (op < 0 || op >= ALU_COUNT) return 0;   /* default / bounds guard */
    return ALU[op](a, b);                       /* the indexed indirect call */
}

/* ------------------------------------------------------------------------- *
 *  P7: evaluate an AArch64 condition code against NZCV flags (models B.cond).
 *  CMP sets NZCV; `B.<cond> label` is TAKEN iff the flags satisfy <cond>.
 *  Here nzcv packs the flags as  N=bit3  Z=bit2  C=bit1  V=bit0.
 * ------------------------------------------------------------------------- */
static bool decode_condition(uint8_t nzcv, uint8_t cond) {
    int N = (nzcv >> 3) & 1;
    int Z = (nzcv >> 2) & 1;
    int C = (nzcv >> 1) & 1;
    int V = (nzcv >> 0) & 1;
    switch (cond & 0xF) {
        case 0x0: return Z;                  /* EQ    equal              */
        case 0x1: return !Z;                 /* NE    not equal          */
        case 0x2: return C;                  /* CS/HS unsigned >=        */
        case 0x3: return !C;                 /* CC/LO unsigned <         */
        case 0x4: return N;                  /* MI    negative           */
        case 0x5: return !N;                 /* PL    positive or zero   */
        case 0x6: return V;                  /* VS    overflow set       */
        case 0x7: return !V;                 /* VC    overflow clear     */
        case 0x8: return C && !Z;            /* HI    unsigned >         */
        case 0x9: return !(C && !Z);         /* LS    unsigned <=        */
        case 0xA: return N == V;             /* GE    signed >=          */
        case 0xB: return N != V;             /* LT    signed <           */
        case 0xC: return !Z && (N == V);     /* GT    signed >           */
        case 0xD: return Z || (N != V);      /* LE    signed <=          */
        case 0xE: return true;               /* AL    always             */
        default:  return true;               /* NV    (0xF) always       */
    }
}

/* ------------------------------------------------------------------------- *
 *  P8: classify a branch by target direction — the #1 disassembly-reading
 *  skill. A BACKWARD edge (target <= the branch's own address) closes a LOOP;
 *  a FORWARD conditional branch skips a block (an IF); a forward unconditional
 *  branch is a goto/break. This is how you find control flow in raw code.
 * ------------------------------------------------------------------------- */
enum branch_kind { BR_LOOP, BR_IF, BR_GOTO };

static enum branch_kind classify_branch(uint64_t pc, uint64_t target, bool conditional) {
    if (target <= pc) return BR_LOOP;     /* backward edge  -> loop        */
    if (conditional)  return BR_IF;       /* forward + cond -> if/skip     */
    return BR_GOTO;                        /* forward uncond -> goto/break  */
}

static const char *branch_kind_name(enum branch_kind k) {
    switch (k) {
        case BR_LOOP: return "LOOP (backward branch)";
        case BR_IF:   return "IF   (forward conditional)";
        case BR_GOTO: return "GOTO (forward unconditional)";
    }
    return "?";
}

/* ------------------------------------------------------------------------- *
 *  P9: resolve an address to a symbol name — and synthesize "sub_XXXX" when
 *  the binary is stripped. nm/readelf read .symtab; `strip` deletes it, so
 *  Ghidra/IDA invent names like sub_401000 from the address. Modelled exactly.
 *  Returns a static name, or writes the synthesized name into `fallback`.
 * ------------------------------------------------------------------------- */
struct sym { uint64_t addr; const char *name; };

static const char *resolve_symbol(uint64_t addr, char *fallback, size_t n) {
    static const struct sym table[] = {
        { 0x400500, "main"        },
        { 0x400620, "verify_pin"  },
        { 0x400700, "sha256_init" },
    };
    for (size_t i = 0; i < sizeof table / sizeof table[0]; i++)
        if (table[i].addr == addr) return table[i].name;
    snprintf(fallback, n, "sub_%06" PRIx64, addr);   /* stripped -> synthesize */
    return fallback;
}

/* ------------------------------------------------------------------------- *
 *  P10: patch a byte in a code buffer — the essence of a crack/fix. Overwrite
 *  one opcode to change behaviour (here MUL 0x03 -> ADD 0x02): the bytecode
 *  analogue of flipping B.EQ<->B.NE or NOP-ing out a license check. Refuses
 *  to write out of bounds. Returns true on success.
 * ------------------------------------------------------------------------- */
static bool patch_bytecode(uint8_t *code, size_t len, size_t off, uint8_t val) {
    if (off >= len) return false;
    code[off] = val;
    return true;
}

/* ------------------------------------------------------------------------- */
int main(void) {
    const uint8_t *prog = SAMPLE_PROG;
    const size_t   plen = sizeof SAMPLE_PROG;

    puts("== Topic 30: ARM Assembly & Reverse Engineering (modelled in portable C) ==\n");

    /* P1 */
    printf("P1  add(2, 3)                     = %d\n", add(2, 3));

    /* P2 */
    printf("P2  sum_to_n(100)                 = %d\n", sum_to_n(100));

    /* P3 */
    {
        const uint8_t le[4] = { 0x78, 0x56, 0x34, 0x12 };
        printf("P3  read_u32_le(78 56 34 12)      = 0x%08" PRIx32 "\n", read_u32_le(le));
    }

    /* hexdump helper + P4 */
    puts("\n--  hexdump(SAMPLE_PROG):");
    hexdump(prog, plen);
    puts("P4  disassembly:");
    disassemble(prog, plen);

    /* P5 (compute into a variable first, then print it) */
    puts("P5  execute (PRINT side-effects shown):");
    int64_t result = vm_execute(prog, plen, true);
    printf("P5  vm result                     = %" PRId64 "\n", result);

    /* P6 */
    printf("P6  alu_dispatch(MUL, 6, 7)        = %ld\n", alu_dispatch(ALU_MUL, 6, 7));

    /* P7: flags with Z=1 -> B.EQ taken, B.NE not taken */
    {
        uint8_t flags = 0x4;                       /* N=0 Z=1 C=0 V=0 */
        bool eq = decode_condition(flags, 0x0);
        bool ne = decode_condition(flags, 0x1);
        printf("P7  Z=1: B.%s taken=%d, B.%s taken=%d\n",
               cond_name(0x0), (int)eq, cond_name(0x1), (int)ne);
    }

    /* P8 */
    {
        enum branch_kind back = classify_branch(0x1040, 0x1020, true);
        enum branch_kind fwd  = classify_branch(0x1040, 0x1080, true);
        printf("P8  0x1040->0x1020: %s\n", branch_kind_name(back));
        printf("P8  0x1040->0x1080: %s\n", branch_kind_name(fwd));
    }

    /* P9 (compute pointers first, then print) */
    {
        char buf[32] = {0};
        const char *known = resolve_symbol(0x400620, buf, sizeof buf);
        printf("P9  resolve 0x400620              = %s\n", known);

        char buf2[32] = {0};
        const char *unknown = resolve_symbol(0x401234, buf2, sizeof buf2);
        printf("P9  resolve 0x401234 (stripped)   = %s\n", unknown);
    }

    /* P10: patch MUL -> ADD, then re-disassemble and re-run to prove it changed */
    {
        uint8_t patched[sizeof SAMPLE_PROG];
        memcpy(patched, SAMPLE_PROG, sizeof patched);
        patch_bytecode(patched, sizeof patched, MUL_OFFSET, OP_ADD);
        puts("P10 after patch MUL->ADD @off 16, disassembly:");
        disassemble(patched, sizeof patched);
        int64_t patched_result = vm_execute(patched, sizeof patched, false);
        printf("P10 patched vm result             = %" PRId64 "  (was %" PRId64 ")\n",
               patched_result, result);
    }

    return 0;
}
