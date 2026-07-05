/*
 * Topic 05: Memory Management
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* P2: Allocate, fill, return a dynamic array (caller frees). */
int *make_array(size_t n) {
    int *p = malloc(n * sizeof *p);
    if (!p) return NULL;               /* always check */
    for (size_t i = 0; i < n; i++) p[i] = (int)i * 10;
    return p;
}

/* P3: Dynamic 2-D array as a single contiguous block (cache-friendly). */
int *make_matrix(size_t rows, size_t cols) {
    int *m = malloc(rows * cols * sizeof *m);
    if (!m) return NULL;
    for (size_t r = 0; r < rows; r++)
        for (size_t c = 0; c < cols; c++)
            m[r * cols + c] = (int)(r * cols + c);
    return m;
}

/* P4: Safe realloc growth idiom (don't clobber original pointer on failure). */
int grow(int **arr, size_t *cap, size_t new_cap) {
    int *tmp = realloc(*arr, new_cap * sizeof **arr);
    if (!tmp) return 0;                /* *arr still valid, must free later */
    *arr = tmp;
    *cap = new_cap;
    return 1;
}

/* P7: Deep copy of a struct holding a pointer. */
typedef struct {
    char *name;                        /* owned */
    int   id;
} Record;

Record deep_copy(const Record *src) {
    Record dst;
    dst.id = src->id;
    dst.name = malloc(strlen(src->name) + 1);
    if (dst.name) strcpy(dst.name, src->name);
    return dst;
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
    p->free_list = NULL;
    for (int i = 0; i < POOL_BLOCKS; i++) {
        *(void **)p->mem[i] = p->free_list;   /* store next ptr in block */
        p->free_list = p->mem[i];
    }
}
void *pool_alloc(Pool *p) {
    if (!p->free_list) return NULL;
    void *blk = p->free_list;
    p->free_list = *(void **)blk;
    return blk;
}
void pool_free(Pool *p, void *blk) {
    *(void **)blk = p->free_list;
    p->free_list = blk;
}

int main(void) {
    /* P2 */
    int *a = make_array(5);
    printf("P2  make_array           -> %d %d %d %d %d\n", a[0],a[1],a[2],a[3],a[4]);
    free(a); a = NULL;                 /* P8: null after free */

    /* P3 */
    size_t R = 2, C = 3;
    int *m = make_matrix(R, C);
    printf("P3  matrix[1][2]         = %d\n", m[1 * C + 2]);
    free(m);

    /* P4 */
    size_t cap = 2;
    int *v = malloc(cap * sizeof *v);
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
