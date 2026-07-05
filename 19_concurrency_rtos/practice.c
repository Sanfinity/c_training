/*
 * Topic 19: Concurrency & RTOS Concepts -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdbool.h>

/* P3: a simple mutex model (owner tracking, like an RTOS mutex). */
typedef struct {
    bool locked;
    int  owner;      /* task id that holds it, -1 if free */
} Mutex;

void mutex_init(Mutex *m) {
    /* TODO: implement. HINT: put the mutex in the free state -- not locked, with
       owner set to a sentinel such as -1. */
}
bool mutex_lock(Mutex *m, int task) {
    /* TODO: implement. HINT: if it is already locked, fail; otherwise record this
       task as the owner and mark it locked. */
    return false; /* placeholder */
}
bool mutex_unlock(Mutex *m, int task) {
    /* TODO: implement. HINT: only the current owner may unlock; then clear the
       locked flag and owner. */
    return false; /* placeholder */
}

/* P4: counting semaphore modeling a pool of N resources. */
typedef struct { int count; } Semaphore;
void sem_init(Semaphore *s, int n) {
    /* TODO: implement. HINT: store the initial number of available resources. */
}
bool sem_take(Semaphore *s) {          /* wait / P */
    /* TODO: implement. HINT: if the count is zero fail (would block), otherwise
       decrement it and succeed. */
    return false; /* placeholder */
}
void sem_give(Semaphore *s) {          /* signal / V */
    /* TODO: implement. HINT: increment the count to signal a resource is free. */
}

/* P2: demonstrate why count++ is a non-atomic read-modify-store.
 * If two tasks interleave load/store, one increment is lost. */
int racy_increment_demo(void) {
    /* TODO: implement. HINT: model two tasks that each load the shared value
       before either stores, so one increment is lost; return the (too small) result. */
    return 0; /* placeholder */
}

/* P6/P7: deadlock avoidance by global lock ordering.
 * Two "tasks" each need both locks; acquiring in the SAME order prevents deadlock. */
bool acquire_ordered(Mutex *a, Mutex *b, int task) {
    /* TODO: implement. HINT: pick a consistent global order for the two locks
       (e.g. by address) so every task acquires them the same way; if the second
       lock fails, release the first. */
    return false; /* placeholder */
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
    /* TODO: implement. HINT: reset head/tail, set the empty semaphore to BUF_N
       and the full semaphore to 0. */
}
bool pc_produce(PC *pc, int item) {
    /* TODO: implement. HINT: take an empty slot (fail if none), store the item,
       advance head modulo BUF_N, then signal the full semaphore. */
    return false; /* placeholder */
}
bool pc_consume(PC *pc, int *item) {
    /* TODO: implement. HINT: take a full slot (fail if none), read the item,
       advance tail modulo BUF_N, then signal the empty semaphore. */
    return false; /* placeholder */
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
    int x = 0, y = 0;   /* initialized so the stub build has no -Wuninitialized warning */
    pc_consume(&pc, &x); pc_consume(&pc, &y);
    printf("P9  produce 4, 5th blocked=%d, consume -> %d %d\n", overflow, x, y);

    /* P8 note */
    printf("P8  priority inversion -> fix with priority inheritance (boost holder)\n");
    return 0;
}
