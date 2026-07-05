/*
 * Topic 19: Concurrency & RTOS Concepts
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * RTOS primitives are SIMULATED single-threaded so this compiles with the
 * standard command (no -lpthread). The logic mirrors real mutex/semaphore use.
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdbool.h>

/* P3: a simple mutex model (owner tracking, like an RTOS mutex). */
typedef struct {
    bool locked;
    int  owner;      /* task id that holds it, -1 if free */
} Mutex;

void mutex_init(Mutex *m) { m->locked = false; m->owner = -1; }
bool mutex_lock(Mutex *m, int task) {
    if (m->locked) return false;   /* would block in a real RTOS */
    m->locked = true; m->owner = task;
    return true;
}
bool mutex_unlock(Mutex *m, int task) {
    if (!m->locked || m->owner != task) return false; /* only owner unlocks */
    m->locked = false; m->owner = -1;
    return true;
}

/* P4: counting semaphore modeling a pool of N resources. */
typedef struct { int count; } Semaphore;
void sem_init(Semaphore *s, int n) { s->count = n; }
bool sem_take(Semaphore *s) {          /* wait / P */
    if (s->count <= 0) return false;   /* would block if 0 */
    s->count--;
    return true;
}
void sem_give(Semaphore *s) { s->count++; } /* signal / V */

/* P2: demonstrate why count++ is a non-atomic read-modify-store.
 * If two tasks interleave load/store, one increment is lost. */
int racy_increment_demo(void) {
    int shared = 0;
    /* Task A loads 0 ... Task B loads 0 ... both store 1 -> lost update. */
    int a_load = shared;     /* 0 */
    int b_load = shared;     /* 0 (B preempted A before A stored) */
    shared = a_load + 1;     /* 1 */
    shared = b_load + 1;     /* 1  <- should have been 2 */
    return shared;           /* returns 1: the lost-update bug */
}

/* P6/P7: deadlock avoidance by global lock ordering.
 * Two "tasks" each need both locks; acquiring in the SAME order prevents deadlock. */
bool acquire_ordered(Mutex *a, Mutex *b, int task) {
    /* always lock the lower-address mutex first -> a consistent global order */
    Mutex *first = a, *second = b;
    if (a > b) { first = b; second = a; }
    if (!mutex_lock(first, task))  return false;
    if (!mutex_lock(second, task)) { mutex_unlock(first, task); return false; }
    return true;
}

/* P9: producer/consumer with a bounded buffer + counting semaphores. */
#define BUF_N 4
typedef struct {
    int buf[BUF_N];
    int head, tail;
    Semaphore empty;   /* counts free slots  */
    Semaphore full;    /* counts used slots  */
} PC;
void pc_init(PC *pc) {
    pc->head = pc->tail = 0;
    sem_init(&pc->empty, BUF_N);
    sem_init(&pc->full, 0);
}
bool pc_produce(PC *pc, int item) {
    if (!sem_take(&pc->empty)) return false;   /* wait for a free slot */
    pc->buf[pc->head] = item;
    pc->head = (pc->head + 1) % BUF_N;
    sem_give(&pc->full);                        /* signal an item is ready */
    return true;
}
bool pc_consume(PC *pc, int *item) {
    if (!sem_take(&pc->full)) return false;    /* wait for an item */
    *item = pc->buf[pc->tail];
    pc->tail = (pc->tail + 1) % BUF_N;
    sem_give(&pc->empty);                       /* signal a free slot */
    return true;
}

int main(void) {
    /* P3 (sequenced: printf arg order is unspecified, so use statements) */
    Mutex m; mutex_init(&m);
    bool t1_lock   = mutex_lock(&m, 1);    /* T1 acquires  -> success */
    bool t2_lock   = mutex_lock(&m, 2);    /* T2 blocked   -> fail    */
    bool t2_unlock = mutex_unlock(&m, 2);  /* only owner may unlock -> fail */
    bool t1_unlock = mutex_unlock(&m, 1);  /* T1 releases  -> success */
    printf("P3  T1 lock=%d, T2 lock=%d, T2 unlock=%d, T1 unlock=%d\n",
           t1_lock, t2_lock, t2_unlock, t1_unlock);

    /* P4 (sequenced for the same reason) */
    Semaphore s; sem_init(&s, 2);
    bool k1 = sem_take(&s);   /* 2 -> 1, success */
    bool k2 = sem_take(&s);   /* 1 -> 0, success */
    bool k3 = sem_take(&s);   /* 0,      fails   */
    printf("P4  sem(2): take=%d take=%d take=%d (3rd fails)\n", k1, k2, k3);

    /* P2 */
    printf("P2  racy count++ result  = %d (should be 2 -> lost update bug)\n",
           racy_increment_demo());

    /* P6/P7 */
    Mutex a, b; mutex_init(&a); mutex_init(&b);
    bool ok = acquire_ordered(&a, &b, 1);
    printf("P7  ordered acquire      = %d (consistent order avoids deadlock)\n", ok);
    mutex_unlock(&a, 1); mutex_unlock(&b, 1);

    /* P9 */
    PC pc; pc_init(&pc);
    for (int i = 1; i <= 4; i++) pc_produce(&pc, i * 10);
    bool overflow = !pc_produce(&pc, 99);   /* buffer full */
    int x, y;
    pc_consume(&pc, &x); pc_consume(&pc, &y);
    printf("P9  produce 4, 5th blocked=%d, consume -> %d %d\n", overflow, x, y);

    /* P8 note */
    printf("P8  priority inversion -> fix with priority inheritance (boost holder)\n");
    return 0;
}
