/*
 * Topic 10: Endianness -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdint.h>

/* P2: detect endianness two ways. */
int is_little_endian_union(void) {
    /* TODO: implement. HINT: put 1 into a uint32_t union and check whether byte[0] holds the 1. */
    return 0; /* placeholder */
}
int is_little_endian_ptr(void) {
    /* TODO: implement. HINT: aim a uint8_t* at a uint32_t holding 1 and read the first byte. */
    return 0; /* placeholder */
}

/* P3/P4/P5: byte swaps. */
uint16_t bswap16(uint16_t v) {
    /* TODO: implement. HINT: swap the high and low bytes of the 16-bit value. */
    return 0; /* placeholder */
}
uint32_t bswap32(uint32_t v) {
    /* TODO: implement. HINT: reverse the order of all four bytes using shifts and masks. */
    return 0; /* placeholder */
}
uint64_t bswap64(uint64_t v) {
    /* TODO: implement. HINT: swap the two 32-bit halves and byte-swap each half. */
    return 0; /* placeholder */
}

/* P7: read a 32-bit big-endian value from a byte buffer (portable, endianness-agnostic). */
uint32_t load_be32(const uint8_t *p) {
    /* TODO: implement. HINT: combine p[0..3] with the FIRST byte as the most-significant. */
    return 0; /* placeholder */
}
uint32_t load_le32(const uint8_t *p) {
    /* TODO: implement. HINT: combine p[0..3] with the FIRST byte as the least-significant. */
    return 0; /* placeholder */
}

/* P8: store a 32-bit value into a buffer in a chosen endianness. */
void store_be32(uint8_t *p, uint32_t v) {
    /* TODO: implement. HINT: write the most-significant byte to p[0] and the least to p[3]. */
}

/* P6: host<->network (16-bit) implemented portably. */
uint16_t my_htons(uint16_t host) {
    /* TODO: implement. HINT: on a little-endian host swap the bytes, otherwise return it unchanged. */
    return 0; /* placeholder */
}

int main(void) {
    printf("P1  0x12345678 stored as -> ");
    union { uint32_t u; uint8_t b[4]; } d = { .u = 0x12345678u };
    for (int i = 0; i < 4; i++) printf("%02X ", d.b[i]);
    printf("(%s-endian)\n", is_little_endian_union() ? "little" : "big");

    printf("P2  detect: union=%d ptr=%d\n", is_little_endian_union(), is_little_endian_ptr());
    printf("P3  bswap16(0x1234)      = 0x%04X\n", bswap16(0x1234));
    printf("P4  bswap32(0x12345678)  = 0x%08X\n", bswap32(0x12345678u));
    printf("P5  bswap64              = 0x%016llX\n",
           (unsigned long long)bswap64(0x0123456789ABCDEFull));

    uint8_t buf[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    printf("P7  load_be32            = 0x%08X\n", load_be32(buf));
    printf("P7  load_le32            = 0x%08X\n", load_le32(buf));

    uint8_t out[4] = {0};   /* practice-template init: store_be32 stub leaves this unwritten */
    store_be32(out, 0xCAFEBABEu);
    printf("P8  store_be32           -> %02X %02X %02X %02X\n", out[0],out[1],out[2],out[3]);

    printf("P6  my_htons(0x00FF)     = 0x%04X (network/big-endian)\n", my_htons(0x00FF));
    return 0;
}
