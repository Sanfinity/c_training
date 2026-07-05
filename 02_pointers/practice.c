/*
 * Topic 02: Pointers -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* P1: Swap two ints through pointers (pass-by-reference). */
void swap(int *a, int *b) {
    /* TODO: implement. HINT: use a temporary to exchange the values that a and b point to. */
}

/* P2: Reverse an array in place with two pointers. */
void reverse_array(int *arr, size_t n) {
    /* TODO: implement. HINT: walk one pointer from the front and one from the back, swapping until they meet. */
}

/* P3: strlen via pointer arithmetic. */
size_t my_strlen(const char *s) {
    /* TODO: implement. HINT: advance a pointer to the terminating '\0', then subtract the start. */
    return 0; /* placeholder */
}

/* P4: Count vowels by walking a string with a pointer. */
int count_vowels(const char *s) {
    /* TODO: implement. HINT: iterate character by character and count a/e/i/o/u in both cases. */
    return 0; /* placeholder */
}

/* P5: const placement demo. p1: data const; p2: pointer const. */
static int demo_const(void) {
    /* TODO: implement. HINT: a `const int *` lets you re-point but not write through it; an `int *const` is the opposite. Combine two such reads. */
    return 0; /* placeholder */
}

/* P8: Allocate inside a function via a double pointer. */
void alloc_and_fill(int **out, size_t n) {
    /* TODO: implement. HINT: write the newly malloc'd block back through *out, then fill the elements. */
}

/* P9/P10: Function pointers and a dispatch table. */
static int f_add(int a, int b) {
    /* TODO: implement. HINT: return the sum. */
    return 0; /* placeholder */
}
static int f_sub(int a, int b) {
    /* TODO: implement. HINT: return the difference. */
    return 0; /* placeholder */
}
static int f_mul(int a, int b) {
    /* TODO: implement. HINT: return the product. */
    return 0; /* placeholder */
}

int apply(int (*op)(int, int), int a, int b) {
    /* TODO: implement. HINT: invoke the function that op points to, passing a and b. */
    return 0; /* placeholder */
}

/* P11: Generic memcpy over void*. */
void *my_memcpy(void *dst, const void *src, size_t n) {
    /* TODO: implement. HINT: copy n bytes one unsigned char at a time from src to dst, then return dst. */
    return NULL; /* placeholder */
}

/* P14: qsort comparator (ascending ints). */
static int cmp_int(const void *a, const void *b) {
    /* TODO: implement. HINT: cast both void* to const int*, then return negative/zero/positive without risking overflow. */
    return 0; /* placeholder */
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
    if (!heap) heap = calloc(5, sizeof *heap);   /* scaffold: avoid NULL deref until P8 is implemented */
    printf("P8  alloc_and_fill       -> %d %d %d %d %d\n",
           heap[0],heap[1],heap[2],heap[3],heap[4]);
    free(heap);

    int (*table[3])(int,int) = { f_add, f_sub, f_mul };
    printf("P9  apply(add,6,4)       = %d\n", apply(f_add, 6, 4));
    printf("P10 dispatch[+,-,*](6,4) = %d %d %d\n",
           table[0](6,4), table[1](6,4), table[2](6,4));

    char dst[16] = {0};
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
