/*
 * Topic 03: Arrays -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <limits.h>
#include <stddef.h>

/* P1: Max and min in one pass. */
void max_min(const int *a, size_t n, int *mx, int *mn) {
    /* TODO: implement. HINT: initialize the max output to the smallest int and the min output to the largest, then update both in one pass. */
}

/* P2: Second largest (returns INT_MIN if none). */
int second_largest(const int *a, size_t n) {
    /* TODO: implement. HINT: track the largest and second largest as you scan; update carefully when a new max appears. */
    return 0; /* placeholder */
}

/* P3: Reverse in place. */
void reverse(int *a, size_t n) {
    /* TODO: implement. HINT: swap the i-th and (n-1-i)-th elements moving inward from both ends. */
}

/* P4: Left-rotate by k using three reversals. */
void left_rotate(int *a, size_t n, size_t k) {
    /* TODO: implement. HINT: reduce k mod n, then reverse [0,k), reverse [k,n), and finally reverse the whole array. */
}

/* P5: Remove duplicates from sorted array; return new length. */
size_t remove_dups(int *a, size_t n) {
    /* TODO: implement. HINT: keep a write index; copy an element only when it differs from the last kept one. */
    return 0; /* placeholder */
}

/* P6: Move zeros to the end, keep order. */
void move_zeros(int *a, size_t n) {
    /* TODO: implement. HINT: compact all non-zero values forward with a write index, then fill the rest with zeros. */
}

/* P7: Missing number in 1..n via XOR (robust vs overflow). */
int missing_number(const int *a, size_t n_present) {
    /* the array holds n_present elements drawn from 1..(n_present+1) */
    /* TODO: implement. HINT: XOR every element together with every value in 1..(n_present+1); the survivor is missing. */
    return 0; /* placeholder */
}

/* P8: Kadane's maximum subarray sum. */
int max_subarray(const int *a, size_t n) {
    /* TODO: implement. HINT: keep a running best-ending-here (restart it when it would be smaller than the element) and a global best. */
    return 0; /* placeholder */
}

/* P9: Two-sum on a sorted array; returns 1 and sets indices if found. */
int two_sum_sorted(const int *a, size_t n, int target, size_t *i, size_t *j) {
    /* TODO: implement. HINT: two indices from both ends; move them inward based on whether the pair sum is under or over target. */
    return 0; /* placeholder */
}

/* P11: Boyer-Moore majority element. */
int majority_element(const int *a, size_t n) {
    /* TODO: implement. HINT: hold a candidate and a counter; reset the candidate when the counter hits zero. */
    return 0; /* placeholder */
}

/* P15: Dutch National Flag (0,1,2 sort in one pass). */
void sort_012(int *a, size_t n) {
    /* TODO: implement. HINT: use lo/mid/hi pointers; push 0s to the front, 2s to the back, and step past 1s. */
}

int main(void) {
    int a[] = {3, 7, 1, 9, 2, 9, 4};
    size_t n = sizeof(a)/sizeof(a[0]);
    int mx = 0, mn = 0; max_min(a, n, &mx, &mn);
    printf("P1  max/min              = %d %d\n", mx, mn);
    printf("P2  second_largest       = %d\n", second_largest(a, n));

    int r[] = {1,2,3,4,5};
    reverse(r, 5);
    printf("P3  reverse              -> %d %d %d %d %d\n", r[0],r[1],r[2],r[3],r[4]);

    int rot[] = {1,2,3,4,5,6,7};
    left_rotate(rot, 7, 3);
    printf("P4  left_rotate by 3     -> %d %d %d %d %d %d %d\n",
           rot[0],rot[1],rot[2],rot[3],rot[4],rot[5],rot[6]);

    int d[] = {1,1,2,2,2,3,4,4};
    size_t nd = remove_dups(d, 8);
    printf("P5  remove_dups len=%zu   -> %d %d %d %d\n", nd, d[0],d[1],d[2],d[3]);

    int z[] = {0,1,0,3,12};
    move_zeros(z, 5);
    printf("P6  move_zeros           -> %d %d %d %d %d\n", z[0],z[1],z[2],z[3],z[4]);

    int m[] = {1,2,4,5};   /* missing 3 out of 1..5 */
    printf("P7  missing_number       = %d\n", missing_number(m, 4));

    int k[] = {-2,1,-3,4,-1,2,1,-5,4};
    printf("P8  max_subarray         = %d\n", max_subarray(k, 9));

    int s[] = {1,3,4,5,7,11};
    size_t i, j;
    if (two_sum_sorted(s, 6, 9, &i, &j))
        printf("P9  two_sum(9)           -> idx %zu,%zu = %d,%d\n", i, j, s[i], s[j]);

    int maj[] = {2,2,1,1,1,2,2};
    printf("P11 majority_element     = %d\n", majority_element(maj, 7));

    int f[] = {2,0,2,1,1,0,2,1,0};
    sort_012(f, 9);
    printf("P15 sort_012             -> %d%d%d%d%d%d%d%d%d\n",
           f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]);
    return 0;
}
