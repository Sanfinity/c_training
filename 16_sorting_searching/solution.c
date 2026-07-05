/*
 * Topic 16: Sorting & Searching
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <string.h>
#include <stddef.h>

static void swap(int *a, int *b) { int t = *a; *a = *b; *b = t; }

/* P1: bubble sort with early exit. */
void bubble_sort(int *a, int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++)
            if (a[j] > a[j + 1]) { swap(&a[j], &a[j + 1]); swapped = 1; }
        if (!swapped) break;
    }
}

/* P3: insertion sort. */
void insertion_sort(int *a, int n) {
    for (int i = 1; i < n; i++) {
        int key = a[i], j = i - 1;
        while (j >= 0 && a[j] > key) { a[j + 1] = a[j]; j--; }
        a[j + 1] = key;
    }
}

/* P4: merge sort. */
static void merge(int *a, int l, int m, int r, int *tmp) {
    int i = l, j = m + 1, k = l;
    while (i <= m && j <= r) tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i <= m) tmp[k++] = a[i++];
    while (j <= r) tmp[k++] = a[j++];
    for (int t = l; t <= r; t++) a[t] = tmp[t];
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

/* P5: quick sort (Lomuto partition). */
static int partition(int *a, int lo, int hi) {
    int pivot = a[hi], i = lo - 1;
    for (int j = lo; j < hi; j++)
        if (a[j] < pivot) swap(&a[++i], &a[j]);
    swap(&a[i + 1], &a[hi]);
    return i + 1;
}
void quick_sort(int *a, int lo, int hi) {
    if (lo < hi) {
        int p = partition(a, lo, hi);
        quick_sort(a, lo, p - 1);
        quick_sort(a, p + 1, hi);
    }
}

/* P6: binary search (iterative, overflow-safe mid). */
int binary_search(const int *a, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == key) return mid;
        if (a[mid] < key) lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}

/* P7: first occurrence (lower bound). */
int first_occurrence(const int *a, int n, int key) {
    int lo = 0, hi = n - 1, res = -1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == key) { res = mid; hi = mid - 1; }
        else if (a[mid] < key) lo = mid + 1;
        else hi = mid - 1;
    }
    return res;
}

/* P9: quickselect - k-th smallest (0-indexed). */
int quickselect(int *a, int lo, int hi, int k) {
    if (lo == hi) return a[lo];
    int p = partition(a, lo, hi);
    if (k == p) return a[p];
    return (k < p) ? quickselect(a, lo, p - 1, k) : quickselect(a, p + 1, hi, k);
}

/* P10: search in rotated sorted array. */
int search_rotated(const int *a, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == key) return mid;
        if (a[lo] <= a[mid]) {                 /* left half sorted */
            if (a[lo] <= key && key < a[mid]) hi = mid - 1; else lo = mid + 1;
        } else {                               /* right half sorted */
            if (a[mid] < key && key <= a[hi]) lo = mid + 1; else hi = mid - 1;
        }
    }
    return -1;
}

/* P11: integer sqrt via binary search. */
int isqrt(int x) {
    if (x < 2) return x;
    long lo = 1, hi = x, ans = 0;
    while (lo <= hi) {
        long mid = lo + (hi - lo) / 2;
        if (mid * mid <= x) { ans = mid; lo = mid + 1; } else hi = mid - 1;
    }
    return (int)ans;
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
