/*
 * Topic 01: Bitwise Operations & Bit Manipulation -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

/* P1: Count set bits (Brian Kernighan) - O(number of set bits). */
int count_set_bits(uint32_t n) {
    /* TODO: implement. HINT: repeatedly clear the lowest set bit and count how many times you can. */
    return 0; /* placeholder */
}

/* P2: Is n a power of two? (exactly one bit set, and n != 0) */
bool is_power_of_two(uint32_t n) {
    /* TODO: implement. HINT: a power of two has a single set bit; think about how n and n-1 relate, and exclude 0. */
    return false; /* placeholder */
}

/* P3: XOR swap without a temporary. Guard against a==b (aliasing) which would zero it. */
void xor_swap(int *a, int *b) {
    /* TODO: implement. HINT: three chained XORs swap two values in place; return early if both pointers alias. */
}

/* P4: Set / clear / toggle / check the k-th bit (0-indexed). */
uint32_t set_bit(uint32_t n, int k) {
    /* TODO: implement. HINT: OR n with a mask that has only bit k set. */
    return 0; /* placeholder */
}
uint32_t clear_bit(uint32_t n, int k) {
    /* TODO: implement. HINT: AND n with the inverse of a mask that has only bit k set. */
    return 0; /* placeholder */
}
uint32_t toggle_bit(uint32_t n, int k) {
    /* TODO: implement. HINT: XOR n with a mask that has only bit k set. */
    return 0; /* placeholder */
}
int check_bit(uint32_t n, int k) {
    /* TODO: implement. HINT: bring bit k down to the lowest position, then keep just that bit. */
    return 0; /* placeholder */
}

/* P5: Reverse the 32 bits of n. */
uint32_t reverse_bits(uint32_t n) {
    /* TODO: implement. HINT: 32 times, shift the result left and append the current lowest bit of n. */
    return 0; /* placeholder */
}

/* P6: Element appearing once when all others appear twice. */
int single_number(const int *arr, int size) {
    /* TODO: implement. HINT: XOR cancels equal pairs, so accumulate the XOR of every element. */
    return 0; /* placeholder */
}

/* P7: Isolate and clear the lowest set bit. */
uint32_t lowest_set_bit(uint32_t n) {
    /* TODO: implement. HINT: n and its two's-complement negation share only the lowest set bit. */
    return 0; /* placeholder */
}
uint32_t clear_lowest_set(uint32_t n) {
    /* TODO: implement. HINT: subtracting one flips the lowest set bit and everything below it. */
    return 0; /* placeholder */
}

/* P8: 1-indexed position of the rightmost set bit (0 if n == 0). */
int rightmost_set_pos(uint32_t n) {
    /* TODO: implement. HINT: handle 0 first; otherwise shift right until the low bit is set, counting steps. */
    return 0; /* placeholder */
}

/* P9: Multiply / divide by 2^p using shifts (arithmetic shift keeps sign). */
int mul_by_pow2(int n, int p) {
    /* TODO: implement. HINT: shifting left by p multiplies by 2^p. */
    return 0; /* placeholder */
}
int div_by_pow2(int n, int p) {
    /* TODO: implement. HINT: shifting right by p divides by 2^p; a signed right shift preserves the sign. */
    return 0; /* placeholder */
}

/* P10: Hamming distance = number of differing bits. */
int hamming_distance(uint32_t a, uint32_t b) {
    /* TODO: implement. HINT: the differing bits are exactly the set bits of a XOR b. */
    return 0; /* placeholder */
}

/* P11: Opposite signs without multiplication (branch-free). */
bool opposite_signs(int a, int b) {
    /* TODO: implement. HINT: combine the two numbers so the result's sign bit tells you if signs differ. */
    return false; /* placeholder */
}

/* P12: Swap the two nibbles of a byte. */
uint8_t swap_nibbles(uint8_t b) {
    /* TODO: implement. HINT: move the low nibble up by 4 and the high nibble down by 4, then combine. */
    return 0; /* placeholder */
}

/* P13: Rotate 32-bit value; masking d avoids UB when d is 0 or a multiple of 32. */
uint32_t rotate_left(uint32_t n, unsigned d) {
    /* TODO: implement. HINT: reduce d modulo 32, then OR a left shift with the complementary right shift. */
    return 0; /* placeholder */
}
uint32_t rotate_right(uint32_t n, unsigned d) {
    /* TODO: implement. HINT: reduce d modulo 32, then OR a right shift with the complementary left shift. */
    return 0; /* placeholder */
}

/* P14: Two elements appearing once; all others twice. */
void two_single_numbers(const int *arr, int size, int *out1, int *out2) {
    /* TODO: implement. HINT: XOR everything, isolate one differing bit, then split elements into two groups by that bit. */
}

/* P15: XOR of 1..n in O(1) using the period-4 pattern. */
uint32_t xor_1_to_n(uint32_t n) {
    /* TODO: implement. HINT: the answer depends only on n mod 4. */
    return 0; /* placeholder */
}

/* P16: Add two ints using only bitwise ops (carry-propagate adder). */
int add_without_plus(int a, int b) {
    /* TODO: implement. HINT: XOR gives the sum ignoring carry; the carry comes from AND shifted left; loop until no carry. */
    return 0; /* placeholder */
}

/* P17: |n| without branching. */
int abs_no_branch(int n) {
    /* TODO: implement. HINT: derive an all-ones-or-zero mask from the sign bit, then combine it with n via add and XOR. */
    return 0; /* placeholder */
}

/* P18: Alternating bit pattern? (no two adjacent bits are equal). */
bool has_alternating_bits(uint32_t n) {
    /* TODO: implement. HINT: XOR n with itself shifted right by one; an alternating pattern makes every bit become 1. */
    return false; /* placeholder */
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
    int r14[] = {2,4,7,9,2,4}; int s1 = 0, s2 = 0; two_single_numbers(r14,6,&s1,&s2);
    printf("P14 two_single_numbers         = %d %d\n", s1, s2);
    printf("P15 xor_1_to_n(7)              = %u\n", xor_1_to_n(7));
    printf("P16 add_without_plus(13,29)    = %d\n", add_without_plus(13,29));
    printf("P17 abs_no_branch(-42)         = %d\n", abs_no_branch(-42));
    printf("P18 has_alternating_bits(0x5)  = %d\n", has_alternating_bits(0x5));
    return 0;
}
