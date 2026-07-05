/*
 * Topic 09: Alignment & Padding
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

/* P3/P4: same members, different order -> different size. */
struct Wasteful { char a; double b; char c; int d; };  /* padding everywhere */
struct Tight    { double b; int d; char a; char c; };  /* packed by hand nicely */

/* P8: packed struct removes padding (GCC/Clang extension). */
struct __attribute__((packed)) Packed { char a; double b; char c; int d; };

/* P9: model a register block with explicit reserved padding. */
struct Regs {
    volatile uint32_t CTRL;      /* 0x00 */
    volatile uint32_t STATUS;    /* 0x04 */
    uint32_t          _rsvd[2];  /* 0x08..0x0F reserved */
    volatile uint32_t DATA;      /* 0x10 */
};

/* P11: align a value up to the next multiple of 'align' (power of two). */
static size_t align_up(size_t x, size_t align) {
    return (x + (align - 1)) & ~(align - 1);
}

/* P10: is a pointer aligned to 'align' bytes? */
static int is_aligned(const void *p, size_t align) {
    return ((uintptr_t)p & (align - 1)) == 0;
}

int main(void) {
    printf("P3  sizeof Wasteful=%zu Tight=%zu (same fields!)\n",
           sizeof(struct Wasteful), sizeof(struct Tight));
    printf("P8  sizeof Packed=%zu (no padding)\n", sizeof(struct Packed));

    printf("P5  alignof(char)=%zu int=%zu double=%zu\n",
           alignof(char), alignof(int), alignof(double));

    /* P5: over-aligned buffer. */
    alignas(16) unsigned char buf[16];
    printf("P5  alignas(16) buf %% 16 = %zu (aligned=%d)\n",
           (uintptr_t)buf % 16, is_aligned(buf, 16));

    printf("P11 align_up(13,8)=%zu align_up(16,8)=%zu align_up(17,16)=%zu\n",
           align_up(13, 8), align_up(16, 8), align_up(17, 16));

    /* P16: offsetof verifies hardware field placement. */
    printf("P9  offsetof DATA=%zu (expect 0x10=%d)\n",
           offsetof(struct Regs, DATA), 0x10);

    printf("P6  sizeof Wasteful is multiple of alignof=%zu -> %s\n",
           alignof(struct Wasteful),
           (sizeof(struct Wasteful) % alignof(struct Wasteful) == 0) ? "yes" : "no");
    return 0;
}
