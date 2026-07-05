/*
 * Topic 07: Structures, Unions & Bit-fields -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/* P2/P3: padding. Poorly ordered vs well ordered. */
struct Bad  { char c; int i; char d; };   /* lots of padding */
struct Good { int i; char c; char d; };   /* less padding    */

/* P5: union to inspect bytes / detect endianness. */
union U32 {
    uint32_t u;
    uint8_t  b[4];
};

/* P6/P14: model a hardware register with a bit-field struct. */
struct CtrlReg {
    uint32_t enable   : 1;
    uint32_t mode     : 3;
    uint32_t speed    : 4;
    uint32_t reserved : 24;
};

/* P7: self-referential struct (list node). */
typedef struct Node {
    int data;
    struct Node *next;
} Node;

/* P9/P10: offsetof and container_of. */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

struct Wrapper {
    int   header;
    Node  node;
    int   footer;
};

/* P8: pass by value vs pointer. */
struct Point { int x, y; };
long by_value(struct Point p) {
    /* TODO: implement. HINT: return the sum of the two members of the by-value copy. */
    return 0; /* placeholder */
}
long by_pointer(const struct Point *p) {
    /* TODO: implement. HINT: return the sum of the two members reached through the pointer. */
    return 0; /* placeholder */
}

/* P14 (alt): same register via masks/shifts instead of bit-fields. */
#define SPEED_SHIFT 4
#define SPEED_MASK  (0xFu << SPEED_SHIFT)
static uint32_t set_speed(uint32_t reg, uint32_t speed) {
    /* TODO: implement. HINT: clear the old SPEED field with the mask, then OR in the shifted new speed. */
    return 0; /* placeholder */
}

int main(void) {
    printf("P2  sizeof Bad=%zu Good=%zu (padding differs)\n",
           sizeof(struct Bad), sizeof(struct Good));

    union U32 u = { .u = 0x01020304u };
    printf("P5  endianness           -> %s (low byte=0x%02X)\n",
           (u.b[0] == 0x04) ? "little" : "big", u.b[0]);

    struct CtrlReg r = {0};
    r.enable = 1; r.mode = 5; r.speed = 9;
    printf("P6  bitfield CtrlReg      -> en=%u mode=%u speed=%u (size=%zu)\n",
           r.enable, r.mode, r.speed, sizeof r);

    Node n3 = {3, NULL}, n2 = {2, &n3}, n1 = {1, &n2};
    printf("P7  list                 -> %d %d %d\n", n1.data, n1.next->data, n1.next->next->data);

    struct Wrapper w = { .header = 0xAA, .node = {99, NULL}, .footer = 0xBB };
    Node *np = &w.node;
    struct Wrapper *back = container_of(np, struct Wrapper, node);
    printf("P9  offsetof(node)=%zu\n", offsetof(struct Wrapper, node));
    printf("P10 container_of         -> header=0x%X footer=0x%X\n", back->header, back->footer);

    struct Point pt = {10, 32};
    printf("P8  by_value=%ld by_pointer=%ld\n", by_value(pt), by_pointer(&pt));

    uint32_t reg = 0;
    reg = set_speed(reg, 0xD);
    printf("P14 mask/shift set_speed  -> 0x%08X (speed field=%u)\n",
           reg, (reg & SPEED_MASK) >> SPEED_SHIFT);
    return 0;
}
