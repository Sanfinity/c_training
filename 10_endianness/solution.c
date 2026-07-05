/*
 * Topic 10: Endianness
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>

/* P2: detect endianness two ways. */
int is_little_endian_union(void) {
    union { uint32_t u; uint8_t b[4]; } x = { .u = 1 };
    return x.b[0] == 1;
}
int is_little_endian_ptr(void) {
    uint32_t u = 1;
    return *(uint8_t *)&u == 1;
}

/* P3/P4/P5: byte swaps. */
uint16_t bswap16(uint16_t v) {
    return (uint16_t)((v >> 8) | (v << 8));
}
uint32_t bswap32(uint32_t v) {
    return ((v & 0x000000FFu) << 24) |
           ((v & 0x0000FF00u) <<  8) |
           ((v & 0x00FF0000u) >>  8) |
           ((v & 0xFF000000u) >> 24);
}
uint64_t bswap64(uint64_t v) {
    return ((uint64_t)bswap32((uint32_t)v) << 32) | bswap32((uint32_t)(v >> 32));
}

/* P7: read a 32-bit big-endian value from a byte buffer (portable, endianness-agnostic). */
uint32_t load_be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] <<  8) |  (uint32_t)p[3];
}
uint32_t load_le32(const uint8_t *p) {
    return ((uint32_t)p[3] << 24) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[1] <<  8) |  (uint32_t)p[0];
}

/* P8: store a 32-bit value into a buffer in a chosen endianness. */
void store_be32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >>  8); p[3] = (uint8_t)v;
}

/* P6: host<->network (16-bit) implemented portably. */
uint16_t my_htons(uint16_t host) {
    return is_little_endian_union() ? bswap16(host) : host;
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

    uint8_t out[4];
    store_be32(out, 0xCAFEBABEu);
    printf("P8  store_be32           -> %02X %02X %02X %02X\n", out[0],out[1],out[2],out[3]);

    printf("P6  my_htons(0x00FF)     = 0x%04X (network/big-endian)\n", my_htons(0x00FF));
    return 0;
}
