/*
 * Topic 14: Stacks & Queues -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* P1: array-based stack. */
#define STACK_CAP 64
typedef struct {
    int data[STACK_CAP];
    int top;                 /* index of next free slot */
} Stack;

void st_init(Stack *s) {
    /* TODO: implement. HINT: reset the top index so the stack starts empty. */
}
bool st_empty(const Stack *s) {
    /* TODO: implement. HINT: empty means top has returned to its starting position. */
    return false; /* placeholder */
}
bool st_full(const Stack *s) {
    /* TODO: implement. HINT: full means top has reached the capacity. */
    return false; /* placeholder */
}
bool st_push(Stack *s, int v) {
    /* TODO: implement. HINT: refuse when full; otherwise store v at top and advance it. Report success. */
    return false; /* placeholder */
}
bool st_pop(Stack *s, int *v) {
    /* TODO: implement. HINT: refuse when empty; otherwise move top down and hand back that value via *v. */
    return false; /* placeholder */
}
int  st_peek(const Stack *s) {
    /* TODO: implement. HINT: return the value just below top without removing it. */
    return 0; /* placeholder */
}

/* P4: circular buffer (ring) - keeps a count to distinguish full/empty. */
#define RING_CAP 5
typedef struct {
    int data[RING_CAP];
    int head, tail, count;
} Ring;

void ring_init(Ring *r) {
    /* TODO: implement. HINT: zero out head, tail and count. */
}
bool ring_full(const Ring *r)  {
    /* TODO: implement. HINT: the ring is full when count equals the capacity. */
    return false; /* placeholder */
}
bool ring_empty(const Ring *r) {
    /* TODO: implement. HINT: the ring is empty when count is zero. */
    return false; /* placeholder */
}
bool ring_put(Ring *r, int v) {
    /* TODO: implement. HINT: reject when full; store at tail, advance tail modulo capacity, bump count. */
    return false; /* placeholder */
}
bool ring_get(Ring *r, int *v) {
    /* TODO: implement. HINT: reject when empty; read from head, advance head modulo capacity, drop count. */
    return false; /* placeholder */
}

/* P6: evaluate a postfix (RPN) expression of single-digit operands. */
int eval_postfix(const char *expr) {
    /* TODO: implement. HINT: push digits; on an operator pop two operands, apply it, push the result. */
    return 0; /* placeholder */
}

/* P8: queue using two stacks (amortized O(1)). */
typedef struct { Stack in, out; } Queue2;
void q2_init(Queue2 *q) {
    /* TODO: implement. HINT: initialize both the in and out stacks. */
}
void q2_enqueue(Queue2 *q, int v) {
    /* TODO: implement. HINT: pushing onto the in stack is enough. */
}
bool q2_dequeue(Queue2 *q, int *v) {
    /* TODO: implement. HINT: when out is empty, drain in into out to reverse order, then pop from out. */
    return false; /* placeholder */
}

/* P10: min-stack - getMin in O(1) via an auxiliary stack. */
typedef struct { Stack main, mins; } MinStack;
void ms_init(MinStack *m) {
    /* TODO: implement. HINT: initialize both the main stack and the mins stack. */
}
void ms_push(MinStack *m, int v) {
    /* TODO: implement. HINT: always push to main; also push to mins when v is a new (<=) minimum. */
}
void ms_pop(MinStack *m) {
    /* TODO: implement. HINT: pop main; if that value equals the current min, pop mins too. */
}
int ms_min(const MinStack *m) {
    /* TODO: implement. HINT: the current minimum sits on top of the mins stack. */
    return 0; /* placeholder */
}

int main(void) {
    Stack s; st_init(&s);
    st_push(&s, 10); st_push(&s, 20); st_push(&s, 30);
    int v = 0; st_pop(&s, &v);
    printf("P1  stack push 10,20,30 pop=%d peek=%d\n", v, st_peek(&s));

    Ring r; ring_init(&r);
    for (int i = 1; i <= 6; i++) printf("P4  ring_put(%d)=%d ", i, ring_put(&r, i));
    printf("\n    ring_get -> ");
    while (ring_get(&r, &v)) printf("%d ", v);
    printf("\n");

    printf("P6  eval_postfix(\"53+82-*\") = %d\n", eval_postfix("53+82-*")); /* (5+3)*(8-2)=48 */

    Queue2 q; q2_init(&q);
    q2_enqueue(&q, 1); q2_enqueue(&q, 2); q2_enqueue(&q, 3);
    printf("P8  queue-from-2-stacks  -> ");
    while (q2_dequeue(&q, &v)) printf("%d ", v);
    printf("\n");

    MinStack m; ms_init(&m);
    ms_push(&m, 5); ms_push(&m, 2); ms_push(&m, 7); ms_push(&m, 1);
    printf("P10 min-stack min=%d ", ms_min(&m));
    ms_pop(&m);
    printf("after pop min=%d\n", ms_min(&m));
    return 0;
}
