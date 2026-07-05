/*
 * Topic 01: Bitwise Operations & Bit Manipulation
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

/* P1: Count set bits (Brian Kernighan) - O(number of set bits). */
int count_set_bits(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);      /* clear the lowest set bit each iteration */
        count++;
    }
    return count;
}

/* P2: Is n a power of two? (exactly one bit set, and n != 0) */
bool is_power_of_two(uint32_t n) {
    return n != 0 && (n & (n - 1)) == 0;
}

/* P3: XOR swap without a temporary. Guard against a==b (aliasing) which would zero it. */
void xor_swap(int *a, int *b) {
    if (a == b) return;
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

/* P4: Set / clear / toggle / check the k-th bit (0-indexed). */
uint32_t set_bit(uint32_t n, int k)    { return n | (1u << k); }
uint32_t clear_bit(uint32_t n, int k)  { return n & ~(1u << k); }
uint32_t toggle_bit(uint32_t n, int k) { return n ^ (1u << k); }
int      check_bit(uint32_t n, int k)  { return (int)((n >> k) & 1u); }

/* P5: Reverse the 32 bits of n. */
uint32_t reverse_bits(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result = (result << 1) | (n & 1u);
        n >>= 1;
    }
    return result;
}

/* P6: Element appearing once when all others appear twice. */
int single_number(const int *arr, int size) {
    int x = 0;
    for (int i = 0; i < size; i++) x ^= arr[i];
    return x;
}

/* P7: Isolate and clear the lowest set bit. */
uint32_t lowest_set_bit(uint32_t n)   { return n & (0u - n); } /* isolate */
uint32_t clear_lowest_set(uint32_t n) { return n & (n - 1); }  /* remove  */

/* P8: 1-indexed position of the rightmost set bit (0 if n == 0). */
int rightmost_set_pos(uint32_t n) {
    if (n == 0) return 0;
    int pos = 1;
    while (!(n & 1u)) { n >>= 1; pos++; }
    return pos;
}

/* P9: Multiply / divide by 2^p using shifts (arithmetic shift keeps sign). */
int mul_by_pow2(int n, int p) { return n << p; }
int div_by_pow2(int n, int p) { return n >> p; }

/* P10: Hamming distance = number of differing bits. */
int hamming_distance(uint32_t a, uint32_t b) { return count_set_bits(a ^ b); }

/* P11: Opposite signs without multiplication (sign bit of a^b). */
bool opposite_signs(int a, int b) { return (a ^ b) < 0; }

/* P12: Swap the two nibbles of a byte. */
uint8_t swap_nibbles(uint8_t b) { return (uint8_t)((b << 4) | (b >> 4)); }

/* P13: Rotate 32-bit value; masking d avoids UB when d is 0 or a multiple of 32. */
uint32_t rotate_left(uint32_t n, unsigned d) {
    d &= 31u;
    return d ? (n << d) | (n >> (32u - d)) : n;
}
uint32_t rotate_right(uint32_t n, unsigned d) {
    d &= 31u;
    return d ? (n >> d) | (n << (32u - d)) : n;
}

/* P14: Two elements appearing once; all others twice. */
void two_single_numbers(const int *arr, int size, int *out1, int *out2) {
    uint32_t xor_all = 0;
    for (int i = 0; i < size; i++) xor_all ^= (uint32_t)arr[i];
    uint32_t diff = xor_all & (0u - xor_all); /* isolate any differing bit */
    int a = 0, b = 0;
    for (int i = 0; i < size; i++) {
        if ((uint32_t)arr[i] & diff) a ^= arr[i];
        else                         b ^= arr[i];
    }
    *out1 = a;
    *out2 = b;
}

/* P15: XOR of 1..n in O(1) using the period-4 pattern. */
uint32_t xor_1_to_n(uint32_t n) {
    switch (n & 3u) {
        case 0:  return n;
        case 1:  return 1;
        case 2:  return n + 1;
        default: return 0;
    }
}

/* P16: Add two ints using only bitwise ops (carry-propagate adder). */
int add_without_plus(int a, int b) {
    while (b != 0) {
        unsigned carry = (unsigned)(a & b) << 1;
        a = a ^ b;
        b = (int)carry;
    }
    return a;
}

/* P17: |n| without branching. mask is all-ones for negative, zero otherwise. */
int abs_no_branch(int n) {
    int mask = n >> (int)(sizeof(int) * CHAR_BIT - 1);
    return (n + mask) ^ mask;
}

/* P18: Alternating bit pattern? x = n ^ (n>>1) becomes all ones iff alternating. */
bool has_alternating_bits(uint32_t n) {
    uint32_t x = n ^ (n >> 1);
    return (x & (x + 1)) == 0;
}

int main(void) {
    printf("P1  count_set_bits(0xB)        = %d\n", count_set_bits(0xB));       /* 3 */
    printf("P2  is_power_of_two(64/63)     = %d %d\n", is_power_of_two(64), is_power_of_two(63));
    int a = 5, b = 9; xor_swap(&a, &b);
    printf("P3  xor_swap(5,9)              -> a=%d b=%d\n", a, b);
    printf("P4  set/clr/tgl/chk            = %u %u %u %d\n",
           set_bit(0,3), clear_bit(0xF,2), toggle_bit(0,3), check_bit(0x8,3));
    printf("P5  reverse_bits(1)            = 0x%08X\n", reverse_bits(1u));
    int r6[] = {4,1,2,1,2};
    printf("P6  single_number              = %d\n", single_number(r6,5));
    printf("P7  isolate/clear low bit(12)  = %u %u\n", lowest_set_bit(12), clear_lowest_set(12));
    printf("P8  rightmost_set_pos(12)      = %d\n", rightmost_set_pos(12));
    printf("P9  3<<2 , 20>>2               = %d %d\n", mul_by_pow2(3,2), div_by_pow2(20,2));
    printf("P10 hamming_distance(1,4)      = %d\n", hamming_distance(1,4));
    printf("P11 opposite_signs(-3,4)       = %d\n", opposite_signs(-3,4));
    printf("P12 swap_nibbles(0xAB)         = 0x%02X\n", swap_nibbles(0xAB));
    printf("P13 rol/ror(0x80000001,1)      = 0x%08X 0x%08X\n",
           rotate_left(0x80000001u,1), rotate_right(0x80000001u,1));
    int r14[] = {2,4,7,9,2,4}; int s1, s2; two_single_numbers(r14,6,&s1,&s2);
    printf("P14 two_single_numbers         = %d %d\n", s1, s2);
    printf("P15 xor_1_to_n(7)              = %u\n", xor_1_to_n(7));
    printf("P16 add_without_plus(13,29)    = %d\n", add_without_plus(13,29));
    printf("P17 abs_no_branch(-42)         = %d\n", abs_no_branch(-42));
    printf("P18 has_alternating_bits(0x5)  = %d\n", has_alternating_bits(0x5));
    return 0;
}
