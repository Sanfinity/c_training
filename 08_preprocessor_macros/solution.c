/*
 * Topic 08: Preprocessor & Macros
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function/macro; main() demonstrates all of them.
 */
#include <stdio.h>

/* P2: fully-parenthesized MIN/MAX/SQUARE. */
#define MAX(a, b)   (((a) > (b)) ? (a) : (b))
#define MIN(a, b)   (((a) < (b)) ? (a) : (b))
#define SQUARE(x)   ((x) * (x))

/* P7: stringize (#) and token-paste (##). */
#define STR(x)      #x
#define XSTR(x)     STR(x)          /* expand then stringize */
#define CONCAT(a,b) a ## b

/* P9: ARRAY_SIZE (breaks if 'a' is a pointer, not an array). */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* P10/P11: CHECK macro using __FILE__/__LINE__, wrapped in do/while(0). */
#define CHECK(cond)                                                     \
    do {                                                               \
        if (!(cond))                                                   \
            printf("CHECK failed: %s at %s:%d\n", #cond, __FILE__, __LINE__); \
    } while (0)

/* P12: debug print that vanishes when NDEBUG is defined. */
#ifdef NDEBUG
#  define DBG(...) ((void)0)
#else
#  define DBG(...) printf(__VA_ARGS__)   /* P17: variadic macro */
#endif

/* P15: register-style bit macros. */
#define SET_BIT(reg, n)   ((reg) |=  (1u << (n)))
#define CLEAR_BIT(reg, n) ((reg) &= ~(1u << (n)))
#define READ_BIT(reg, n)  (((reg) >> (n)) & 1u)

/* P16: X-Macro - single source list generates enum + name table. */
#define COLOR_LIST  X(RED) X(GREEN) X(BLUE)
enum Color {
#define X(name) COLOR_##name,
    COLOR_LIST
#undef X
    COLOR_COUNT
};
static const char *color_names[] = {
#define X(name) #name,
    COLOR_LIST
#undef X
};

int main(void) {
    printf("P2  MAX(3,9)=%d MIN(3,9)=%d SQUARE(5)=%d\n",
           MAX(3,9), MIN(3,9), SQUARE(5));

    /* P3: double-evaluation pitfall demonstrated. */
    int i = 5, j = 6;
    int m = MAX(i++, j++);      /* j incremented twice! */
    printf("P3  MAX(i++,j++)=%d then i=%d j=%d (j bumped twice)\n", m, i, j);

    printf("P7  STR=%s CONCAT=%d\n", STR(hello), CONCAT(12, 34));
    printf("P7  XSTR(__LINE__)=%s\n", XSTR(__LINE__));

    int arr[7];
    printf("P9  ARRAY_SIZE           = %zu\n", ARRAY_SIZE(arr));

    printf("P8  __func__=%s __LINE__=%d\n", __func__, __LINE__);

    CHECK(1 + 1 == 2);
    CHECK(2 + 2 == 5);          /* prints a failure line */

    DBG("P12 DBG print (enabled unless NDEBUG)\n");

    unsigned reg = 0;
    SET_BIT(reg, 3); SET_BIT(reg, 5);
    CLEAR_BIT(reg, 3);
    printf("P15 reg=0x%02X bit5=%u bit3=%u\n", reg, READ_BIT(reg,5), READ_BIT(reg,3));

    printf("P16 X-Macro colors       -> ");
    for (int c = 0; c < COLOR_COUNT; c++) printf("%s ", color_names[c]);
    printf("(count=%d)\n", COLOR_COUNT);
    return 0;
}
