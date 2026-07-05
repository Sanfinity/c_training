/*
 * Topic 08: Preprocessor & Macros -- PRACTICE TEMPLATE
 * Fill in each macro marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>

/* P2: fully-parenthesized MIN/MAX/SQUARE.
 * TODO: parenthesize EVERY argument and the whole body so precedence is safe. */
#define MAX(a, b)   (0)          /* TODO */
#define MIN(a, b)   (0)          /* TODO */
#define SQUARE(x)   (0)          /* TODO */

/* P7: stringize (#) and token-paste (##).
 * TODO: STR turns its arg into a "string literal"; XSTR expands then stringizes
 * (so XSTR(__LINE__) gives the number); CONCAT pastes two tokens into one. */
#define STR(x)      ""           /* TODO: use # */
#define XSTR(x)     ""           /* TODO: expand then STR(x) */
#define CONCAT(a,b) 0            /* TODO: use ## */

/* P9: ARRAY_SIZE (breaks if 'a' is a pointer, not an array).
 * TODO: element count = total bytes / bytes-per-element. */
#define ARRAY_SIZE(a) ((size_t)(sizeof(a) * 0))   /* TODO */

/* P10/P11: CHECK macro using __FILE__/__LINE__, wrapped in do/while(0).
 * TODO: if !(cond), printf "CHECK failed: <cond> at <file>:<line>".
 * Stringize cond with #cond; use the __FILE__ and __LINE__ predefined macros. */
#define CHECK(cond)  do { (void)(cond); } while (0)   /* TODO */

/* P12/P17: variadic debug print that vanishes when NDEBUG is defined.
 * TODO: forward __VA_ARGS__ to printf; expand to nothing under NDEBUG. */
#ifdef NDEBUG
#  define DBG(...) ((void)0)
#else
#  define DBG(...) ((void)0)     /* TODO: printf(__VA_ARGS__) */
#endif

/* P15: register-style bit macros.
 * TODO: set / clear / read bit n of reg using (1u << n) and shifts/masks. */
#define SET_BIT(reg, n)   ((void)0)                 /* TODO */
#define CLEAR_BIT(reg, n) ((void)0)                 /* TODO */
#define READ_BIT(reg, n)  (((reg) >> (n)) & 0u)     /* TODO */

/* P16: X-Macro - single source list generates enum + name table.
 * (Provided as a worked reference: study how ONE list drives both the enum
 * and the parallel string table without ever duplicating the names.) */
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
    int m = MAX(i++, j++);      /* j incremented twice with a correct MAX! */
    printf("P3  MAX(i++,j++)=%d then i=%d j=%d (j bumped twice)\n", m, i, j);

    printf("P7  STR=%s CONCAT=%d\n", STR(hello), CONCAT(12, 34));
    printf("P7  XSTR(__LINE__)=%s\n", XSTR(__LINE__));

    int arr[7];
    printf("P9  ARRAY_SIZE           = %zu\n", ARRAY_SIZE(arr));

    printf("P8  __func__=%s __LINE__=%d\n", __func__, __LINE__);

    CHECK(1 + 1 == 2);
    CHECK(2 + 2 == 5);          /* a correct CHECK prints a failure line here */

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
