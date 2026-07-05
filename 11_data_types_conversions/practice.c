/*
 * Topic 11: Data Types, Integer Promotion & Conversions -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

/* P2: the signed/unsigned comparison trap. */
int neg_one_less_than_1u(void) {
    /* TODO: implement. HINT: return the result of comparing -1 with 1u; recall the int side converts to unsigned. */
    return 0; /* placeholder */
}

/* P5: unsigned wraps (defined); signed overflow is UB (avoid). */
unsigned unsigned_wrap(void) {
    /* TODO: implement. HINT: subtract 1 from an unsigned zero and return it; the result wraps modulo 2^N. */
    return 0; /* placeholder */
}

/* P10: compare floats with an epsilon instead of ==. */
int nearly_equal(double a, double b, double eps) {
    /* TODO: implement. HINT: test the absolute difference against eps scaled by the larger magnitude of a and b. */
    return 0; /* placeholder */
}

/* P15: negative int -> unsigned is modulo 2^N. */
unsigned neg_to_unsigned(int n) {
    /* TODO: implement. HINT: a single conversion of n to unsigned is all that is needed. */
    return 0; /* placeholder */
}

/* P3: char arithmetic is promoted to int. */
int char_promotion(void) {
    /* TODO: implement. HINT: add two char values each holding 100; the sum is evaluated in int, so no char overflow. */
    return 0; /* placeholder */
}

int main(void) {
    printf("P1  sizeof char=%zu short=%zu int=%zu long=%zu llong=%zu ptr=%zu\n",
           sizeof(char), sizeof(short), sizeof(int),
           sizeof(long), sizeof(long long), sizeof(void*));

    printf("P2  (-1 < 1u)            = %d (surprisingly false)\n", neg_one_less_than_1u());
    printf("P3  char 100+100         = %d (promoted to int)\n", char_promotion());
    printf("P5  0u - 1              = %u (unsigned wrap)\n", unsigned_wrap());

    /* P6: truncation. */
    int big = 0x1234ABCD;
    uint8_t small = (uint8_t)big;
    printf("P6  truncate 0x%X -> 0x%02X\n", (unsigned)big, small);

    /* P9: float<->int. */
    printf("P9  (int)3.99=%d  (float)16777217 loses precision=%.1f\n",
           (int)3.99, (float)16777217);

    /* P10: 0.1 + 0.2 != 0.3 */
    printf("P10 0.1+0.2==0.3? direct=%d  epsilon=%d\n",
           (0.1 + 0.2 == 0.3), nearly_equal(0.1 + 0.2, 0.3, 1e-12));

    /* P8: char signedness. */
    char c = -1;
    printf("P8  (char)-1 as unsigned = %u, as signed = %d\n",
           (unsigned char)c, c);

    /* P15 */
    printf("P15 (unsigned)-1         = %u\n", neg_to_unsigned(-1));

    /* P14: INT_MIN edge. */
    printf("P14 INT_MIN=%d  (careful: -INT_MIN overflows)\n", INT_MIN);

    /* P16: _Bool normalizes. */
    _Bool flag = 42;
    printf("P16 _Bool from 42        = %d (normalized to 1)\n", flag);

    /* P11: size_t reverse-loop pitfall (done correctly here). */
    int arr[] = {10, 20, 30};
    printf("P11 reverse via size_t   -> ");
    for (size_t i = 3; i-- > 0; ) printf("%d ", arr[i]);  /* safe idiom */
    printf("\n");
    return 0;
}
