/*
 * Topic 05: Memory Management -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* P2: Allocate, fill, return a dynamic array (caller frees). */
int *make_array(size_t n) {
    /* TODO: implement. HINT: malloc n ints, check for NULL, fill them, and return the pointer (caller frees). */
    return NULL; /* placeholder */
}

/* P3: Dynamic 2-D array as a single contiguous block (cache-friendly). */
int *make_matrix(size_t rows, size_t cols) {
    /* TODO: implement. HINT: malloc rows*cols ints and index element (r,c) as r*cols + c. */
    return NULL; /* placeholder */
}

/* P4: Safe realloc growth idiom (don't clobber original pointer on failure). */
int grow(int **arr, size_t *cap, size_t new_cap) {
    /* TODO: implement. HINT: realloc into a TEMP pointer first; only write it back to *arr (and update *cap) if it succeeded. */
    return 0; /* placeholder */
}

/* P7: Deep copy of a struct holding a pointer. */
typedef struct {
    char *name;                        /* owned */
    int   id;
} Record;

Record deep_copy(const Record *src) {
    /* TODO: implement. HINT: copy id directly, but allocate a SEPARATE buffer for name and copy the text so the copy owns its own string. */
    Record dst = { NULL, 0 };
    dst.name = malloc(1);              /* placeholder: empty owned string so main can print/free safely */
    if (dst.name) dst.name[0] = '\0';
    return dst; /* placeholder */
}

/* P10: Fixed-block memory pool (no malloc after init) - classic embedded question.
 * Free list threaded through the blocks themselves. */
#define POOL_BLOCKS 4
#define POOL_BSIZE  16
typedef struct Pool {
    unsigned char mem[POOL_BLOCKS][POOL_BSIZE];
    void *free_list;
} Pool;

void pool_init(Pool *p) {
    /* TODO: implement. HINT: build a singly-linked free list by storing each block's "next" pointer inside the block itself. */
}
void *pool_alloc(Pool *p) {
    /* TODO: implement. HINT: pop the head block off the free list and return it, or NULL when the pool is empty. */
    return NULL; /* placeholder */
}
void pool_free(Pool *p, void *blk) {
    /* TODO: implement. HINT: push blk back onto the front of the free list. */
}

int main(void) {
    /* P2 */
    int *a = make_array(5);
    if (!a) a = calloc(5, sizeof *a);      /* scaffold: avoid NULL deref until P2 is implemented */
    printf("P2  make_array           -> %d %d %d %d %d\n", a[0],a[1],a[2],a[3],a[4]);
    free(a); a = NULL;                 /* P8: null after free */

    /* P3 */
    size_t R = 2, C = 3;
    int *m = make_matrix(R, C);
    if (!m) m = calloc(R * C, sizeof *m);  /* scaffold: avoid NULL deref until P3 is implemented */
    printf("P3  matrix[1][2]         = %d\n", m[1 * C + 2]);
    free(m);

    /* P4 */
    size_t cap = 2;
    int *v = calloc(4, sizeof *v);     /* scaffold: room for 4 ints so main can read v[2]/v[3] before grow (P4) works */
    v[0] = 1; v[1] = 2;
    if (grow(&v, &cap, 4)) { v[2] = 3; v[3] = 4; }
    printf("P4  grow cap=%zu          -> %d %d %d %d\n", cap, v[0],v[1],v[2],v[3]);
    free(v);

    /* P7 */
    Record orig = { NULL, 42 };
    orig.name = malloc(8); strcpy(orig.name, "sensor");
    Record cpy = deep_copy(&orig);
    strcpy(orig.name, "XXXXXX");       /* mutate original */
    printf("P7  deep_copy            -> orig=%s copy=%s (independent)\n",
           orig.name, cpy.name);
    free(orig.name); free(cpy.name);

    /* P10 */
    Pool pool;
    pool_init(&pool);
    void *b1 = pool_alloc(&pool);
    void *b2 = pool_alloc(&pool);
    printf("P10 pool_alloc x2        -> %s %s\n",
           b1 ? "ok" : "null", b2 ? "ok" : "null");
    pool_free(&pool, b1);
    void *b3 = pool_alloc(&pool);      /* reuses freed block */
    printf("P10 reuse after free     -> %s\n", (b3 == b1) ? "same block" : "diff");
    pool_free(&pool, b2);
    pool_free(&pool, b3);

    /* P12: malloc(0) is implementation-defined. */
    void *z = malloc(0);
    printf("P12 malloc(0)            -> %s\n", z ? "non-NULL" : "NULL");
    free(z);
    return 0;
}
