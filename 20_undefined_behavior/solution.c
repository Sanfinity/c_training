/*
 * Topic 20: Undefined Behavior (UB)
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * This file shows the CORRECT / DEFINED patterns and labels the traps in
 * comments. It deliberately does NOT execute UB. Each problem is a standalone
 * function; main() demonstrates the safe versions.
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

/* P2: signed overflow is UB. DEFINED way: check before adding. */
bool safe_add(int a, int b, int *result) {
    if (b > 0 && a > INT_MAX - b) return false;   /* would overflow */
    if (b < 0 && a < INT_MIN - b) return false;   /* would underflow */
    *result = a + b;
    return true;
}

/* P2 (alt): use unsigned when you WANT wraparound (defined modulo 2^N). */
uint32_t wrap_add(uint32_t a, uint32_t b) { return a + b; }

/* P7: shifting. UB if count >= width or shifting into sign bit of signed.
 * DEFINED: use unsigned and keep the count in range. */
uint32_t safe_shl(uint32_t v, unsigned n) {
    if (n >= 32) return 0;      /* avoid UB of shifting by >= width */
    return v << n;
}

/* P9/P16: strict-aliasing-safe type punning via memcpy (compiles to a mov). */
float bits_to_float(uint32_t bits) {
    float f;
    memcpy(&f, &bits, sizeof f);   /* defined; NOT *(float*)&bits */
    return f;
}
uint32_t float_to_bits(float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof bits);
    return bits;
}

/* P12: integer division traps. DEFINED guard. */
bool safe_div(int a, int b, int *q) {
    if (b == 0) return false;                 /* div by zero is UB */
    if (a == INT_MIN && b == -1) return false;/* INT_MIN/-1 overflows */
    *q = a / b;
    return true;
}

int main(void) {
    /* P1 explanation */
    printf("P1  UB=anything may happen | unspecified=one of several valid |\n");
    printf("    impl-defined=documented per compiler\n");

    /* P2 */
    int r;
    printf("P2  safe_add(INT_MAX,1)  = %s (overflow refused)\n",
           safe_add(INT_MAX, 1, &r) ? "ok" : "refused");
    bool add_ok = safe_add(20, 22, &r);   /* compute before reading r */
    printf("P2  safe_add(20,22)      = %s r=%d\n", add_ok ? "ok" : "refused", r);
    printf("P2  wrap_add(0xFFFFFFFF,1) = %u (unsigned wraps, defined)\n",
           wrap_add(0xFFFFFFFFu, 1));

    /* P7 */
    printf("P7  safe_shl(1,31)=0x%08X  safe_shl(1,32)=0x%08X (no UB)\n",
           safe_shl(1, 31), safe_shl(1, 32));

    /* P9 */
    uint32_t bits = 0x40490FDBu;             /* IEEE-754 for ~pi */
    printf("P9  bits_to_float(0x%08X) = %.6f (memcpy pun)\n", bits, bits_to_float(bits));
    printf("P9  float_to_bits(1.0f)   = 0x%08X\n", float_to_bits(1.0f));

    /* P12 */
    int q;
    printf("P12 safe_div(10,0)       = %s\n", safe_div(10, 0, &q) ? "ok" : "refused");
    printf("P12 safe_div(INT_MIN,-1) = %s\n", safe_div(INT_MIN, -1, &q) ? "ok" : "refused");
    bool div_ok = safe_div(20, 4, &q);    /* compute before reading q */
    printf("P12 safe_div(20,4)       = %s q=%d\n", div_ok ? "ok" : "refused", q);

    /* P14 warning */
    printf("P14 compilers ASSUME no UB -> they may DELETE your overflow check!\n");
    printf("P15 build with -fsanitize=undefined,address to catch UB at runtime\n");
    return 0;
}
