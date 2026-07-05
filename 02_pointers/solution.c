/*
 * Topic 02: Pointers
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* P1: Swap two ints through pointers (pass-by-reference). */
void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/* P2: Reverse an array in place with two pointers. */
void reverse_array(int *arr, size_t n) {
    int *lo = arr, *hi = arr + n - 1;
    while (lo < hi) {
        int t = *lo; *lo = *hi; *hi = t;
        lo++; hi--;
    }
}

/* P3: strlen via pointer arithmetic. */
size_t my_strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (size_t)(p - s);
}

/* P4: Count vowels by walking a string with a pointer. */
int count_vowels(const char *s) {
    int count = 0;
    for (const char *p = s; *p; p++) {
        char c = *p;
        if (c=='a'||c=='e'||c=='i'||c=='o'||c=='u'||
            c=='A'||c=='E'||c=='I'||c=='O'||c=='U') count++;
    }
    return count;
}

/* P5: const placement demo. p1: data const; p2: pointer const. */
static int demo_const(void) {
    int x = 10, y = 20;
    const int *p1 = &x;   /* can change p1, cannot change *p1 */
    int *const p2 = &x;   /* can change *p2, cannot change p2 */
    p1 = &y;              /* OK */
    *p2 = 99;             /* OK, x becomes 99 */
    return *p1 + *p2;     /* 20 + 99 = 119 */
}

/* P8: Allocate inside a function via a double pointer. */
void alloc_and_fill(int **out, size_t n) {
    *out = malloc(n * sizeof(int));
    for (size_t i = 0; i < n; i++) (*out)[i] = (int)(i * i);
}

/* P9/P10: Function pointers and a dispatch table. */
static int f_add(int a, int b) { return a + b; }
static int f_sub(int a, int b) { return a - b; }
static int f_mul(int a, int b) { return a * b; }

int apply(int (*op)(int, int), int a, int b) { return op(a, b); }

/* P11: Generic memcpy over void*. */
void *my_memcpy(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dst;
}

/* P14: qsort comparator (ascending ints). */
static int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);   /* avoids overflow of x - y */
}

int main(void) {
    int a = 3, b = 7;
    swap(&a, &b);
    printf("P1  swap(3,7)            -> a=%d b=%d\n", a, b);

    int arr[] = {1,2,3,4,5};
    reverse_array(arr, 5);
    printf("P2  reverse_array        -> %d %d %d %d %d\n",
           arr[0],arr[1],arr[2],arr[3],arr[4]);

    printf("P3  my_strlen(\"embedded\") = %zu\n", my_strlen("embedded"));
    printf("P4  count_vowels(\"Qualcomm\") = %d\n", count_vowels("Qualcomm"));
    printf("P5  demo_const           = %d\n", demo_const());

    int *heap = NULL;
    alloc_and_fill(&heap, 5);
    printf("P8  alloc_and_fill       -> %d %d %d %d %d\n",
           heap[0],heap[1],heap[2],heap[3],heap[4]);
    free(heap);

    int (*table[3])(int,int) = { f_add, f_sub, f_mul };
    printf("P9  apply(add,6,4)       = %d\n", apply(f_add, 6, 4));
    printf("P10 dispatch[+,-,*](6,4) = %d %d %d\n",
           table[0](6,4), table[1](6,4), table[2](6,4));

    char dst[16];
    my_memcpy(dst, "hello", 6);
    printf("P11 my_memcpy            -> %s\n", dst);

    int u[] = {5,2,9,1,7,3};
    qsort(u, 6, sizeof(u[0]), cmp_int);
    printf("P14 qsort                -> %d %d %d %d %d %d\n",
           u[0],u[1],u[2],u[3],u[4],u[5]);

    /* P16: pointer difference yields element count. */
    printf("P16 &u[4]-&u[1]          = %td\n", &u[4] - &u[1]);
    /* P17: sizeof(ptr) vs sizeof(*ptr). */
    printf("P17 sizeof(int*)=%zu sizeof(*int*)=%zu\n", sizeof(int*), sizeof(int));
    return 0;
}
