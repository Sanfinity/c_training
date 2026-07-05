/*
 * Topic 35: C Depth: Variadics, setjmp, Allocators, Generics -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* P1: sum a COUNT-prefixed list of ints with <stdarg.h>. Variadics carry NO
 * type or arity information, so the caller MUST tell us how many args follow --
 * there is otherwise no way to know when to stop. Pulling the wrong type with
 * va_arg is undefined behaviour (this is why variadics are type-unsafe). */
int sum_ints(int count, ...) {
    /* TODO: implement. HINT: va_start(ap,count); loop count times adding va_arg(ap,int); va_end(ap). */
    return 0; /* placeholder */
}

/* P2: average of a count-prefixed int list; returns 0.0 for an empty list to
 * dodge divide-by-zero. Remember the DEFAULT ARGUMENT PROMOTIONS: a float arg
 * arrives as double and char/short as int, so you must va_arg the PROMOTED type. */
double avg_ints(int count, ...) {
    /* TODO: implement. HINT: guard count<=0; sum the va_arg(ap,int) values, then divide by count as a double. */
    return 0.0; /* placeholder */
}

/* P3: a tiny, type-unsafe printf built on <stdarg.h>. Handles %d %s %c %x, the
 * %% escape, and literal text. The FORMAT STRING is the only thing telling
 * va_arg which type to pull for each conversion -- a wrong specifier reads the
 * wrong type. Returns the number of characters written. */
int mini_printf(const char *fmt, ...) {
    /* TODO: implement. HINT: walk fmt; on '%' read the next char and switch on d/x/c/s/%, using va_arg for each; count chars. */
    (void)fmt;
    return 0; /* placeholder */
}

/* P4: try/catch-style error unwinding with setjmp/longjmp.
 * setjmp saves the machine context and returns 0 on the direct call; a later
 * longjmp jumps BACK to that setjmp, which then returns the (nonzero) value it
 * was given -- the "caught" path. checked_div() "throws" on divide-by-zero.
 * Any local modified between setjmp and longjmp whose value is used afterwards
 * must be 'volatile', or it may be clobbered by the register rewind.
 * (longjmp into a function that has ALREADY RETURNED is undefined behaviour.) */
static jmp_buf g_jmp;

int checked_div(int a, int b) {
    if (b == 0) longjmp(g_jmp, 2);        /* "throw": unwind to the setjmp point */
    return a / b;
}

int try_divide(int a, int b) {
    /* TODO: implement. HINT: 'volatile int r'; if setjmp(g_jmp)==0 r=checked_div(a,b) else r=-1; return r. */
    (void)a; (void)b;
    return 0; /* placeholder */
}

/* P5: an arena / bump allocator -- hand out slices of a FIXED buffer by simply
 * advancing an offset. 'align' (a power of two) rounds the offset UP so every
 * block satisfies its type's alignment; a misaligned load/store is UB on some
 * ISAs and slow on others. Returns NULL when the arena is exhausted. No malloc. */
typedef struct { unsigned char *base; size_t size; size_t offset; } Arena;

void *arena_alloc(Arena *a, size_t nbytes, size_t align) {
    /* TODO: implement. HINT: round offset up with (off+(align-1)) & ~(align-1); if it won't fit return NULL; else bump and return base+aligned. */
    (void)a; (void)nbytes; (void)align;
    return NULL; /* placeholder */
}

/* P6: reset the arena -- free EVERYTHING at once by rewinding the offset to 0.
 * O(1) with no per-object bookkeeping; that bulk lifetime IS the point of an
 * arena. (Existing pointers into it are now dangling -- caller's contract.) */
void arena_reset(Arena *a) {
    /* TODO: implement. HINT: one line -- set a->offset back to 0. */
    (void)a;
}

/* helper (non-Pn): the naive forward copy a hand-rolled memcpy would use --
 * it CORRUPTS an overlapping move where dst>src (used to contrast with P7). */
static void naive_copy(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
}

/* P7: a memmove that survives OVERLAPPING regions. If dst sits ABOVE src in the
 * same block, a forward copy would overwrite source bytes before reading them;
 * so pick the copy DIRECTION from the pointer order -- forward when dst<src,
 * backward when dst>src. (memcpy assumes NO overlap and may use either.) */
void *my_memmove(void *dst, const void *src, size_t n) {
    /* TODO: implement. HINT: cast to unsigned char*; if dst<src copy forward, else copy backward (i=n..1); return dst. */
    (void)src; (void)n;
    return dst; /* placeholder (buffer left unchanged) */
}

/* P8: data-driven dispatch via a table of function pointers indexed by opcode.
 * Instead of a switch, index an ARRAY of function pointers -- adding an
 * operation becomes adding a table row while the call site is unchanged. An
 * out-of-range opcode is rejected through the *ok out-parameter. */
typedef int (*CalcFn)(int, int);
enum { OP_ADD, OP_SUB, OP_MUL, OP_DIV };

static inline int op_add(int a, int b) { return a + b; }
static inline int op_sub(int a, int b) { return a - b; }
static inline int op_mul(int a, int b) { return a * b; }
static inline int op_div(int a, int b) { return b ? a / b : 0; }

int calc_dispatch(int op, int a, int b, bool *ok) {
    /* TODO: implement. HINT: static const CalcFn table[]={op_add,op_sub,op_mul,op_div}; range-check op into *ok, else return table[op](a,b). */
    (void)op; (void)a; (void)b; (void)ok;
    return 0; /* placeholder */
}

/* P9: C11 _Generic -- compile-time, type-directed dispatch. GMAX(a,b) picks
 * gmax_int or gmax_dbl from the TYPE of 'a' (resolved at compile time, zero
 * runtime cost). This is the mechanism behind <tgmath.h>. */
static int gmax_int(int a, int b) {
    /* TODO: implement. HINT: return the larger of a and b. */
    (void)a; (void)b;
    return 0; /* placeholder */
}
static double gmax_dbl(double a, double b) {
    /* TODO: implement. HINT: return the larger of a and b. */
    (void)a; (void)b;
    return 0.0; /* placeholder */
}
#define GMAX(a, b) _Generic((a), int: gmax_int, double: gmax_dbl, default: gmax_int)(a, b)

/* comparator (non-Pn helper) for qsort: DESCENDING order. qsort hands you
 * 'const void *' -- cast back to the real element type. Return <0 / 0 / >0;
 * the (a<b)-(a>b) form avoids the overflow of a naive 'b - a'. */
static inline int cmp_int_desc(const void *pa, const void *pb) {
    int a = *(const int *)pa, b = *(const int *)pb;
    return (a < b) - (a > b);
}

/* P10: sort in place with the stdlib qsort plus a CALLBACK comparator (a
 * function pointer). qsort is generic over element type and size and delegates
 * all ordering decisions to your callback -- the classic C callback idiom. */
void sort_desc(int *a, size_t n) {
    /* TODO: implement. HINT: one call -- qsort(a, n, sizeof a[0], cmp_int_desc). */
    (void)a; (void)n;
}

/* P11: recover the enclosing struct from a pointer to one of its MEMBERS using
 * offsetof + container_of -- the trick behind intrusive containers (e.g. the
 * Linux kernel list). offsetof gives the member's byte offset; subtract it from
 * the member's address to reach the struct's address. */
struct Node { int id; const char *name; double weight; };

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

struct Node *node_from_weight(double *wp) {
    /* TODO: implement. HINT: return container_of(wp, struct Node, weight). */
    (void)wp;
    return NULL; /* placeholder */
}

int main(void) {
    /* P1 */
    int s = sum_ints(4, 10, 20, 30, 40);
    printf("P1  sum_ints(4,...)     = %d\n", s);

    /* P2 */
    double avg = avg_ints(4, 10, 20, 30, 45);
    printf("P2  avg_ints(4,...)     = %.2f\n", avg);

    /* P3 */
    printf("P3  mini_printf         = ");
    int nw = mini_printf("int=%d str=%s chr=%c hex=%x %%done\n", 42, "hi", 'Z', 0xBEEFu);
    printf("P3  mini_printf wrote   = %d chars\n", nw);

    /* P4 */
    int q1 = try_divide(20, 4);
    int q2 = try_divide(5, 0);              /* divide-by-zero -> longjmp -> -1 */
    printf("P4  try_divide          = 20/4=%d  5/0=%d (caught via longjmp)\n", q1, q2);

    /* P5 + P6 */
    _Alignas(int) unsigned char storage[128];
    Arena arena = { storage, sizeof storage, 0 };
    (void)arena_alloc(&arena, 1, 1);        /* waste 1 byte so the int must re-align */
    int *ip = arena_alloc(&arena, sizeof(int), _Alignof(int));
    if (ip) *ip = 42;
    int aligned = ((uintptr_t)ip % _Alignof(int)) == 0;
    size_t used = arena.offset;             /* capture BEFORE the reset */
    arena_reset(&arena);
    printf("P5  arena_alloc         = ptr=%s val=%d aligned=%s used=%zu\n",
           ip ? "ok" : "null", ip ? *ip : 0, aligned ? "yes" : "no", used);
    printf("P6  arena_reset         = offset now %zu (bulk O(1) free)\n", arena.offset);

    /* P7 */
    char mm[] = "ABCDEFGH";
    my_memmove(mm + 2, mm, 5);              /* overlapping: shift "ABCDE" to index 2 */
    char naive[] = "ABCDEFGH";
    naive_copy(naive + 2, naive, 5);        /* naive forward copy corrupts the overlap */
    printf("P7  my_memmove overlap  = memmove=\"%s\"  naive=\"%s\"\n", mm, naive);

    /* P8 */
    bool ok1 = false, ok2 = false;
    int r1 = calc_dispatch(OP_MUL, 6, 7, &ok1);
    int r2 = calc_dispatch(99, 1, 1, &ok2);  /* bad opcode -> ok2 stays false */
    printf("P8  calc_dispatch       = 6*7=%d(ok=%s)  op99=%d(ok=%s)\n",
           r1, ok1 ? "y" : "n", r2, ok2 ? "y" : "n");

    /* P9 */
    int gi = GMAX(3, 9);
    double gd = GMAX(2.5, 1.5);
    printf("P9  _Generic GMAX       = max(3,9)=%d  max(2.5,1.5)=%.1f\n", gi, gd);

    /* P10 */
    int arr[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    size_t an = sizeof arr / sizeof arr[0];
    sort_desc(arr, an);
    printf("P10 sort_desc (qsort cb)= ");
    for (size_t i = 0; i < an; i++) printf("%d%s", arr[i], i + 1 < an ? " " : "\n");

    /* P11 */
    struct Node node = { .id = 7, .name = "widget", .weight = 3.5 };
    struct Node *back = node_from_weight(&node.weight);
    printf("P11 container_of        = id=%d name=%s match=%s\n",
           back ? back->id : -1, back ? back->name : "(null)",
           back == &node ? "yes" : "no");

    return 0;
}
