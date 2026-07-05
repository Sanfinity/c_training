/*
 * Topic 11: Data Types, Integer Promotion & Conversions
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

/* P2: the signed/unsigned comparison trap. */
int neg_one_less_than_1u(void) {
    /* -1 is converted to a huge unsigned, so the comparison is FALSE.
     * The warning below is EXACTLY the point; we silence it deliberately. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
    return (-1 < 1u);        /* returns 0 */
#pragma GCC diagnostic pop
}

/* P5: unsigned wraps (defined); signed overflow is UB (avoid). */
unsigned unsigned_wrap(void) {
    unsigned u = 0;
    return u - 1;            /* UINT_MAX, well-defined modulo 2^N */
}

/* P10: compare floats with an epsilon instead of ==. */
int nearly_equal(double a, double b, double eps) {
    return fabs(a - b) <= eps * fmax(1.0, fmax(fabs(a), fabs(b)));
}

/* P15: negative int -> unsigned is modulo 2^N. */
unsigned neg_to_unsigned(int n) {
    return (unsigned)n;
}

/* P3: char arithmetic is promoted to int. */
int char_promotion(void) {
    char a = 100, b = 100;
    return a + b;            /* 200, computed in int (no char overflow) */
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
