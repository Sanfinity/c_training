/*
 * Topic 16: Sorting & Searching -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <string.h>
#include <stddef.h>

static inline void swap(int *a, int *b) { int t = *a; *a = *b; *b = t; }

/* P1: bubble sort with early exit. */
void bubble_sort(int *a, int n) {
    /* TODO: implement. HINT: repeatedly sweep adjacent pairs, swapping any that are
       out of order (swap() is provided); stop early once a full pass makes no swap. */
}

/* P3: insertion sort. */
void insertion_sort(int *a, int n) {
    /* TODO: implement. HINT: for each element, shift the larger elements of the
       already-sorted prefix one slot right, then drop the key into the gap. */
}

/* P4: merge sort. */
static void merge(int *a, int l, int m, int r, int *tmp) {
    /* TODO: implement. HINT: merge the two sorted runs a[l..m] and a[m+1..r] into
       tmp in order, then copy tmp[l..r] back into a. (merge_sort/merge_sort_rec
       below are provided and call this.) */
}
static void merge_sort_rec(int *a, int l, int r, int *tmp) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    merge_sort_rec(a, l, m, tmp);
    merge_sort_rec(a, m + 1, r, tmp);
    merge(a, l, m, r, tmp);
}
void merge_sort(int *a, int n) {
    int tmp[128];
    merge_sort_rec(a, 0, n - 1, tmp);
}

/* P5: quick sort (Lomuto partition). quickselect (P9) reuses partition(). */
static inline int partition(int *a, int lo, int hi) {
    /* TODO: implement. HINT: take a[hi] as the pivot, scan lo..hi-1 moving every
       element smaller than the pivot to the left (swap() is provided), then place
       the pivot after them and return its final index. */
    return lo; /* placeholder */
}
void quick_sort(int *a, int lo, int hi) {
    /* TODO: implement. HINT: if lo<hi, p = partition(a,lo,hi), then recurse on
       a[lo..p-1] and a[p+1..hi]. */
}

/* P6: binary search (iterative, overflow-safe mid). */
int binary_search(const int *a, int n, int key) {
    /* TODO: implement. HINT: keep a lo/hi window; compare the middle element and
       halve the window each step; use lo+(hi-lo)/2 for the mid. Return -1 if absent. */
    return 0; /* placeholder */
}

/* P7: first occurrence (lower bound). */
int first_occurrence(const int *a, int n, int key) {
    /* TODO: implement. HINT: binary search, but on a match record the index and keep
       searching the LEFT half to find the earliest one. */
    return 0; /* placeholder */
}

/* P9: quickselect - k-th smallest (0-indexed). */
int quickselect(int *a, int lo, int hi, int k) {
    /* TODO: implement. HINT: partition() (provided above) puts the pivot at its final
       index p; recurse only into the side that still contains position k. */
    return 0; /* placeholder */
}

/* P10: search in rotated sorted array. */
int search_rotated(const int *a, int n, int key) {
    /* TODO: implement. HINT: binary search, but each step first decide which half is
       sorted, then test whether key falls inside that sorted half. */
    return 0; /* placeholder */
}

/* P11: integer sqrt via binary search. */
int isqrt(int x) {
    /* TODO: implement. HINT: binary search the answer over [1, x]; compare mid*mid to
       x using a wider (long) type to avoid overflow. */
    return 0; /* placeholder */
}

static void print_arr(const char *label, const int *a, int n) {
    printf("%s", label);
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");
}

int main(void) {
    int base[] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    int n = 9, a[9];

    memcpy(a, base, sizeof base); bubble_sort(a, n);    print_arr("P1  bubble    -> ", a, n);
    memcpy(a, base, sizeof base); insertion_sort(a, n); print_arr("P3  insertion -> ", a, n);
    memcpy(a, base, sizeof base); merge_sort(a, n);     print_arr("P4  merge     -> ", a, n);
    memcpy(a, base, sizeof base); quick_sort(a, 0, n-1);print_arr("P5  quick     -> ", a, n);

    printf("P6  binary_search(7)     = idx %d\n", binary_search(a, n, 7));

    int dups[] = {1,2,2,2,3,4,4,5};
    printf("P7  first_occurrence(2)  = idx %d\n", first_occurrence(dups, 8, 2));

    memcpy(a, base, sizeof base);
    printf("P9  3rd smallest (k=2)   = %d\n", quickselect(a, 0, n-1, 2));

    int rot[] = {6,7,8,9,1,2,3,4,5};
    printf("P10 search_rotated(3)    = idx %d\n", search_rotated(rot, 9, 3));
    printf("P11 isqrt(50)            = %d\n", isqrt(50));
    return 0;
}
