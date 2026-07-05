/*
 * Topic 06: Storage Classes & Type Qualifiers
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>

/* P3: static global -> internal linkage, visible only in this file. */
static int g_file_private = 100;

/* P6: const ARRAY / read-only lookup table. 'static const' puts this in .rodata
 * (flash on an MCU), NOT RAM -- the idiomatic way to store constant tables on
 * embedded targets. Any write such as crc4_table[0] = 1; is a COMPILE error. */
static const uint8_t crc4_table[4] = {0x00, 0x07, 0x0E, 0x09};

/* P9: a normal global lives in .data (init != 0) or .bss (init 0 / uninit). */
int g_data = 7;          /* .data  */
int g_bss;               /* .bss   (zero-initialized) */

/* P2: static local retains its value across calls (a call counter). */
int next_id(void) {
    static int id = 0;   /* initialized once, lifetime = whole program */
    return ++id;
}

/* P6/P7: const and const volatile parameters.
 * const volatile models a read-only hardware status register:
 * const  -> our code must not write it
 * volatile -> hardware may change it, so re-read every access. */
int read_status(const volatile int *status_reg) {
    return *status_reg;  /* always an actual load, never cached */
}

/* P16: casting away const and modifying is UB if the object is truly const.
 * Here the underlying object is non-const, so it is well-defined. */
void modify_via_cast(const int *p) {
    *(int *)p = 555;
}

/* P6: passing an array as a const parameter promises the callee will NOT modify
 * the caller's array. 'const int a[]' and 'const int *a' mean the same thing for
 * a parameter; a[i] = 0; inside here would fail to compile. */
long sum_readonly(const int a[], size_t n) {
    long s = 0;
    for (size_t i = 0; i < n; i++) s += a[i];
    return s;
}

/* P5/P15: without 'volatile' the optimizer could hoist the load out of the loop
 * and spin forever. This function shows the correct pattern (bounded here so it
 * terminates in the demo). In real firmware the flag is set by an ISR. */
int wait_for_flag(volatile int *flag, int max_spins) {
    int spins = 0;
    while (*flag == 0 && spins < max_spins) spins++;
    return spins;
}

int main(void) {
    int id1 = next_id();
    int id2 = next_id();
    int id3 = next_id();
    printf("P2  next_id x3           = %d %d %d\n", id1, id2, id3);
    printf("P3  g_file_private       = %d\n", g_file_private);
    printf("P9  g_data(.data)=%d g_bss(.bss)=%d\n", g_data, g_bss);

    int reg = 0x2A;
    printf("P6  read_status          = %d\n", read_status(&reg));

    printf("P6  crc4_table(.rodata)  = {%u,%u,%u,%u}\n",
           (unsigned)crc4_table[0], (unsigned)crc4_table[1],
           (unsigned)crc4_table[2], (unsigned)crc4_table[3]);
    int nums[] = {2, 4, 6, 8};
    printf("P6  sum_readonly(const[])= %ld\n", sum_readonly(nums, 4));

    int x = 1;
    modify_via_cast(&x);
    printf("P16 modify_via_cast      -> %d\n", x);

    int flag = 1;   /* pretend the ISR already set it */
    printf("P5  wait_for_flag        -> spun %d times\n", wait_for_flag(&flag, 1000));

    /* P14 note: volatile != atomic. A volatile ++ is still load-modify-store,
     * so it is NOT safe against concurrent access; you need atomics/locks. */
    printf("P14 volatile != atomic (needs locks/atomics for concurrency)\n");
    return 0;
}
