/*
 * Topic 14: Stacks & Queues
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
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

void st_init(Stack *s)          { s->top = 0; }
bool st_empty(const Stack *s)   { return s->top == 0; }
bool st_full(const Stack *s)    { return s->top == STACK_CAP; }
bool st_push(Stack *s, int v)   { if (st_full(s)) return false; s->data[s->top++] = v; return true; }
bool st_pop(Stack *s, int *v)   { if (st_empty(s)) return false; *v = s->data[--s->top]; return true; }
int  st_peek(const Stack *s)    { return s->data[s->top - 1]; }

/* P4: circular buffer (ring) - keeps a count to distinguish full/empty. */
#define RING_CAP 5
typedef struct {
    int data[RING_CAP];
    int head, tail, count;
} Ring;

void ring_init(Ring *r) { r->head = r->tail = r->count = 0; }
bool ring_full(const Ring *r)  { return r->count == RING_CAP; }
bool ring_empty(const Ring *r) { return r->count == 0; }
bool ring_put(Ring *r, int v) {
    if (ring_full(r)) return false;
    r->data[r->tail] = v;
    r->tail = (r->tail + 1) % RING_CAP;
    r->count++;
    return true;
}
bool ring_get(Ring *r, int *v) {
    if (ring_empty(r)) return false;
    *v = r->data[r->head];
    r->head = (r->head + 1) % RING_CAP;
    r->count--;
    return true;
}

/* P6: evaluate a postfix (RPN) expression of single-digit operands. */
int eval_postfix(const char *expr) {
    Stack s; st_init(&s);
    for (const char *p = expr; *p; p++) {
        if (*p >= '0' && *p <= '9') {
            st_push(&s, *p - '0');
        } else if (*p == ' ') {
            continue;
        } else {
            int b = 0, a = 0; st_pop(&s, &b); st_pop(&s, &a);
            int r = 0;
            switch (*p) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/': r = b ? a / b : 0; break;
            }
            st_push(&s, r);
        }
    }
    int res = 0; st_pop(&s, &res);
    return res;
}

/* P8: queue using two stacks (amortized O(1)). */
typedef struct { Stack in, out; } Queue2;
void q2_init(Queue2 *q) { st_init(&q->in); st_init(&q->out); }
void q2_enqueue(Queue2 *q, int v) { st_push(&q->in, v); }
bool q2_dequeue(Queue2 *q, int *v) {
    if (st_empty(&q->out)) {
        int t;
        while (st_pop(&q->in, &t)) st_push(&q->out, t);
    }
    return st_pop(&q->out, v);
}

/* P10: min-stack - getMin in O(1) via an auxiliary stack. */
typedef struct { Stack main, mins; } MinStack;
void ms_init(MinStack *m) { st_init(&m->main); st_init(&m->mins); }
void ms_push(MinStack *m, int v) {
    st_push(&m->main, v);
    if (st_empty(&m->mins) || v <= st_peek(&m->mins)) st_push(&m->mins, v);
}
void ms_pop(MinStack *m) {
    int v = 0; st_pop(&m->main, &v);
    if (!st_empty(&m->mins) && v == st_peek(&m->mins)) { int t; st_pop(&m->mins, &t); }
}
int ms_min(const MinStack *m) { return st_peek(&m->mins); }

int main(void) {
    Stack s; st_init(&s);
    st_push(&s, 10); st_push(&s, 20); st_push(&s, 30);
    int v; st_pop(&s, &v);
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
