/*
 * Topic 03: Arrays
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <limits.h>
#include <stddef.h>

/* P1: Max and min in one pass. */
void max_min(const int *a, size_t n, int *mx, int *mn) {
    *mx = INT_MIN; *mn = INT_MAX;
    for (size_t i = 0; i < n; i++) {
        if (a[i] > *mx) *mx = a[i];
        if (a[i] < *mn) *mn = a[i];
    }
}

/* P2: Second largest (returns INT_MIN if none). */
int second_largest(const int *a, size_t n) {
    int first = INT_MIN, second = INT_MIN;
    for (size_t i = 0; i < n; i++) {
        if (a[i] > first)      { second = first; first = a[i]; }
        else if (a[i] > second && a[i] < first) second = a[i];
    }
    return second;
}

/* P3: Reverse in place. */
void reverse(int *a, size_t n) {
    for (size_t i = 0, j = n - 1; i < j; i++, j--) {
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

/* P4: Left-rotate by k using three reversals. */
void left_rotate(int *a, size_t n, size_t k) {
    if (n == 0) return;
    k %= n;
    reverse(a, k);
    reverse(a + k, n - k);
    reverse(a, n);
}

/* P5: Remove duplicates from sorted array; return new length. */
size_t remove_dups(int *a, size_t n) {
    if (n == 0) return 0;
    size_t w = 1;
    for (size_t i = 1; i < n; i++)
        if (a[i] != a[w - 1]) a[w++] = a[i];
    return w;
}

/* P6: Move zeros to the end, keep order. */
void move_zeros(int *a, size_t n) {
    size_t w = 0;
    for (size_t i = 0; i < n; i++)
        if (a[i] != 0) a[w++] = a[i];
    while (w < n) a[w++] = 0;
}

/* P7: Missing number in 1..n via XOR (robust vs overflow). */
int missing_number(const int *a, size_t n_present) {
    /* array has n_present elements from 1..(n_present+1) */
    int x = 0;
    for (size_t i = 0; i < n_present; i++) x ^= a[i];
    for (int i = 1; i <= (int)n_present + 1; i++) x ^= i;
    return x;
}

/* P8: Kadane's maximum subarray sum. */
int max_subarray(const int *a, size_t n) {
    int best = a[0], cur = a[0];
    for (size_t i = 1; i < n; i++) {
        cur = (a[i] > cur + a[i]) ? a[i] : cur + a[i];
        if (cur > best) best = cur;
    }
    return best;
}

/* P9: Two-sum on a sorted array; returns 1 and sets indices if found. */
int two_sum_sorted(const int *a, size_t n, int target, size_t *i, size_t *j) {
    size_t lo = 0, hi = n - 1;
    while (lo < hi) {
        int s = a[lo] + a[hi];
        if (s == target) { *i = lo; *j = hi; return 1; }
        if (s < target) lo++; else hi--;
    }
    return 0;
}

/* P11: Boyer-Moore majority element. */
int majority_element(const int *a, size_t n) {
    int cand = a[0], count = 0;
    for (size_t i = 0; i < n; i++) {
        if (count == 0) cand = a[i];
        count += (a[i] == cand) ? 1 : -1;
    }
    return cand;
}

/* P15: Dutch National Flag (0,1,2 sort in one pass). */
void sort_012(int *a, size_t n) {
    size_t lo = 0, mid = 0, hi = n - 1;
    while (mid <= hi) {
        if (a[mid] == 0)      { int t=a[lo]; a[lo]=a[mid]; a[mid]=t; lo++; mid++; }
        else if (a[mid] == 1) { mid++; }
        else                  { int t=a[mid]; a[mid]=a[hi]; a[hi]=t; if (hi==0) break; hi--; }
    }
}

int main(void) {
    int a[] = {3, 7, 1, 9, 2, 9, 4};
    size_t n = sizeof(a)/sizeof(a[0]);
    int mx, mn; max_min(a, n, &mx, &mn);
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
