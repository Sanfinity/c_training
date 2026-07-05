/*
 * Topic 34: Data Structures & Algorithms: Hashing, Heaps, Graphs, DP
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* ===================================================================== */
/* Hashing: a hash table with OPEN ADDRESSING (linear probing).          */
/* Fixed-capacity static storage -> no malloc, nothing to leak.          */
/* Each slot is EMPTY, USED, or DELETED (a "tombstone").                  */
/* ===================================================================== */
#define HT_CAP 16
typedef enum { SLOT_EMPTY = 0, SLOT_USED, SLOT_DEL } SlotState;
typedef struct { char key[16]; int value; SlotState state; } Slot;
typedef struct { Slot slots[HT_CAP]; } HashTable;

/* P1: djb2 string hash.
 * Folds each byte into a running accumulator with h = h*33 + c, i.e.
 * ((h<<5)+h)+c, seeded at 5381. A good hash spreads keys uniformly across
 * buckets so that collisions (two keys landing on the same slot) are rare.
 * Cost O(length of the string). */
unsigned long djb2_hash(const char *s) {
    unsigned long h = 5381UL;
    int c;
    while ((c = (unsigned char)*s++) != 0)
        h = ((h << 5) + h) + (unsigned long)c;   /* h * 33 + c */
    return h;
}

/* P2: insert / update (open addressing, linear probing).
 * Start at the home slot hash%cap and step forward on collision. Update in
 * place if the key already exists, otherwise drop into the first EMPTY or
 * DELETED slot seen. Average O(1); it degrades as the LOAD FACTOR n/cap rises
 * (real tables rehash to a bigger array past ~0.7). Returns false if full. */
bool ht_insert(HashTable *t, const char *key, int value) {
    int start = (int)(djb2_hash(key) % HT_CAP);
    int first_free = -1;
    for (int i = 0; i < HT_CAP; i++) {
        int idx = (start + i) % HT_CAP;
        Slot *s = &t->slots[idx];
        if (s->state == SLOT_USED) {
            if (strcmp(s->key, key) == 0) { s->value = value; return true; }
        } else {
            if (first_free < 0) first_free = idx;      /* remember 1st reusable */
            if (s->state == SLOT_EMPTY) break;         /* key can't be past here */
        }
    }
    if (first_free < 0) return false;                  /* table is full */
    Slot *s = &t->slots[first_free];
    int j = 0;
    for (; j + 1 < (int)sizeof s->key && key[j]; j++) s->key[j] = key[j];
    s->key[j] = '\0';
    s->value = value;
    s->state = SLOT_USED;
    return true;
}

/* P3: lookup. Probe from the home slot; a match writes *out and returns true.
 * Stop at the first EMPTY slot -- the key cannot lie beyond it. Average O(1),
 * worst O(cap) under heavy clustering. */
bool ht_get(const HashTable *t, const char *key, int *out) {
    int start = (int)(djb2_hash(key) % HT_CAP);
    for (int i = 0; i < HT_CAP; i++) {
        int idx = (start + i) % HT_CAP;
        const Slot *s = &t->slots[idx];
        if (s->state == SLOT_EMPTY) return false;
        if (s->state == SLOT_USED && strcmp(s->key, key) == 0) {
            *out = s->value;
            return true;
        }
    }
    return false;
}

/* P4: delete. Mark the slot DELETED (a tombstone) rather than EMPTY, so probe
 * chains that ran THROUGH this slot are not broken for other keys. */
bool ht_delete(HashTable *t, const char *key) {
    int start = (int)(djb2_hash(key) % HT_CAP);
    for (int i = 0; i < HT_CAP; i++) {
        int idx = (start + i) % HT_CAP;
        Slot *s = &t->slots[idx];
        if (s->state == SLOT_EMPTY) return false;
        if (s->state == SLOT_USED && strcmp(s->key, key) == 0) {
            s->state = SLOT_DEL;                        /* tombstone */
            return true;
        }
    }
    return false;
}

/* ===================================================================== */
/* Binary MIN-HEAP as an array-backed priority queue.                    */
/* Node i: parent (i-1)/2, children 2i+1 and 2i+2. Complete tree, so it  */
/* packs into an array with NO pointers.                                 */
/* ===================================================================== */
#define HEAP_CAP 32
typedef struct { int data[HEAP_CAP]; int size; } MinHeap;

/* P5: push -> sift-up. Append at the end, then swap upward while the new
 * value is smaller than its parent. O(log n). */
bool heap_push(MinHeap *h, int val) {
    if (h->size >= HEAP_CAP) return false;
    int i = h->size++;
    h->data[i] = val;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent] <= h->data[i]) break;
        int tmp = h->data[parent]; h->data[parent] = h->data[i]; h->data[i] = tmp;
        i = parent;
    }
    return true;
}

/* P6: pop-min -> sift-down. The root (index 0) is the minimum. Copy it out,
 * move the last element to the root, shrink, then sift down toward the smaller
 * child until the heap property is restored. O(log n). */
bool heap_pop_min(MinHeap *h, int *out) {
    if (h->size == 0) return false;
    *out = h->data[0];
    h->data[0] = h->data[--h->size];
    int i = 0;
    for (;;) {
        int l = 2 * i + 1, r = 2 * i + 2, smallest = i;
        if (l < h->size && h->data[l] < h->data[smallest]) smallest = l;
        if (r < h->size && h->data[r] < h->data[smallest]) smallest = r;
        if (smallest == i) break;
        int tmp = h->data[i]; h->data[i] = h->data[smallest]; h->data[smallest] = tmp;
        i = smallest;
    }
    return true;
}

/* P7: peek. The minimum is always at index 0 -- read it without removing. O(1). */
bool heap_peek(const MinHeap *h, int *out) {
    if (h->size == 0) return false;
    *out = h->data[0];
    return true;
}

/* ===================================================================== */
/* GRAPH as a fixed-size adjacency list: adj[u] holds u's neighbours,    */
/* deg[u] how many. Space O(V+E) (a matrix would be O(V^2)).             */
/* ===================================================================== */
#define MAXV 16
typedef struct { int adj[MAXV][MAXV]; int deg[MAXV]; int n; } Graph;

/* P8: add an UNDIRECTED edge -> append each endpoint to the other's list. */
bool graph_add_edge(Graph *g, int u, int v) {
    if (u < 0 || v < 0 || u >= g->n || v >= g->n) return false;
    if (g->deg[u] >= MAXV || g->deg[v] >= MAXV) return false;
    g->adj[u][g->deg[u]++] = v;
    g->adj[v][g->deg[v]++] = u;
    return true;
}

/* P9: BFS -> visit order via a FIFO queue. Explores level by level, so it
 * finds the fewest-hops (shortest) path in an unweighted graph. Fills order[]
 * and returns the count. O(V+E). */
int graph_bfs(const Graph *g, int start, int *order) {
    if (start < 0 || start >= g->n) return 0;
    bool seen[MAXV] = { false };
    int queue[MAXV], head = 0, tail = 0, cnt = 0;
    seen[start] = true;
    queue[tail++] = start;
    while (head < tail) {
        int u = queue[head++];
        order[cnt++] = u;
        for (int i = 0; i < g->deg[u]; i++) {
            int w = g->adj[u][i];
            if (!seen[w]) { seen[w] = true; queue[tail++] = w; }
        }
    }
    return cnt;
}

/* P10: DFS -> visit order via an EXPLICIT stack (avoids recursion depth
 * limits). Goes as deep as possible before backtracking; the basis for cycle
 * detection and topological sort. Fills order[] and returns the count. O(V+E). */
int graph_dfs(const Graph *g, int start, int *order) {
    if (start < 0 || start >= g->n) return 0;
    bool seen[MAXV] = { false };
    int stack[MAXV * MAXV], top = 0, cnt = 0;
    stack[top++] = start;
    while (top > 0) {
        int u = stack[--top];
        if (seen[u]) continue;              /* may have been queued twice */
        seen[u] = true;
        order[cnt++] = u;
        /* push neighbours in reverse so the lowest index is popped first */
        for (int i = g->deg[u] - 1; i >= 0; i--) {
            int w = g->adj[u][i];
            if (!seen[w]) stack[top++] = w;
        }
    }
    return cnt;
}

/* P11: count CONNECTED COMPONENTS. Flood (BFS) from every not-yet-seen vertex;
 * each fresh flood marks exactly one component. O(V+E). */
int graph_connected_components(const Graph *g) {
    bool seen[MAXV] = { false };
    int queue[MAXV], components = 0;
    for (int s = 0; s < g->n; s++) {
        if (seen[s]) continue;
        components++;
        int head = 0, tail = 0;
        seen[s] = true;
        queue[tail++] = s;
        while (head < tail) {
            int u = queue[head++];
            for (int i = 0; i < g->deg[u]; i++) {
                int w = g->adj[u][i];
                if (!seen[w]) { seen[w] = true; queue[tail++] = w; }
            }
        }
    }
    return components;
}

/* ===================================================================== */
/* DYNAMIC PROGRAMMING. DP applies when a problem has OPTIMAL SUBSTRUCTURE */
/* (the best answer is built from best sub-answers) and OVERLAPPING        */
/* SUBPROBLEMS (the same sub-answers recur). We tabulate bottom-up.        */
/* ===================================================================== */
#define COIN_MAX_AMOUNT 128
#define DP_MAXN         64
#define ED_MAX          64

/* P12: coin change -> fewest coins that sum to 'amount' (-1 if impossible).
 * dp[a] = min over coins of dp[a-coin]+1, with dp[0]=0. amount+1 is the
 * "infinity" sentinel. Time O(amount * #coins), space O(amount). */
int coin_change(const int *coins, int n, int amount) {
    if (amount < 0 || amount > COIN_MAX_AMOUNT) return -1;
    int dp[COIN_MAX_AMOUNT + 1];
    dp[0] = 0;
    for (int a = 1; a <= amount; a++) {
        dp[a] = amount + 1;                            /* sentinel = unreachable */
        for (int i = 0; i < n; i++)
            if (coins[i] <= a && dp[a - coins[i]] + 1 < dp[a])
                dp[a] = dp[a - coins[i]] + 1;
    }
    return dp[amount] > amount ? -1 : dp[amount];
}

/* P13: longest increasing subsequence (LIS) length.
 * dp[i] = longest increasing run ending at i = 1 + max(dp[j]) for j<i with
 * a[j] < a[i]. Answer is the largest dp[i]. Time O(n^2), space O(n).
 * (A heap/binary-search variant reaches O(n log n).) */
int lis_length(const int *a, int n) {
    if (n <= 0) return 0;
    if (n > DP_MAXN) n = DP_MAXN;
    int dp[DP_MAXN];
    int best = 1;
    for (int i = 0; i < n; i++) {
        dp[i] = 1;
        for (int j = 0; j < i; j++)
            if (a[j] < a[i] && dp[j] + 1 > dp[i]) dp[i] = dp[j] + 1;
        if (dp[i] > best) best = dp[i];
    }
    return best;
}

/* P14: edit distance (Levenshtein) -> min insert/delete/substitute operations
 * to turn a into b. dp[i][j] works over prefixes: equal last chars cost
 * nothing (dp[i-1][j-1]); otherwise 1 + min(delete, insert, substitute).
 * Time and space O(len(a) * len(b)). */
int edit_distance(const char *a, const char *b) {
    int la = (int)strlen(a), lb = (int)strlen(b);
    if (la > ED_MAX) la = ED_MAX;
    if (lb > ED_MAX) lb = ED_MAX;
    int dp[ED_MAX + 1][ED_MAX + 1];
    for (int i = 0; i <= la; i++) dp[i][0] = i;         /* delete i chars */
    for (int j = 0; j <= lb; j++) dp[0][j] = j;         /* insert j chars */
    for (int i = 1; i <= la; i++) {
        for (int j = 1; j <= lb; j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                int del = dp[i - 1][j], ins = dp[i][j - 1], sub = dp[i - 1][j - 1];
                int m = del < ins ? del : ins;
                if (sub < m) m = sub;
                dp[i][j] = m + 1;
            }
        }
    }
    return dp[la][lb];
}

int main(void) {
    /* ---- Hashing: P1-P4 ---- */
    unsigned long h1 = djb2_hash("cat");
    unsigned long h2 = djb2_hash("dog");
    printf("P1  djb2_hash            = cat=%lu dog=%lu\n", h1, h2);

    HashTable ht = { 0 };
    ht_insert(&ht, "cat", 10);
    ht_insert(&ht, "dog", 20);
    ht_insert(&ht, "bird", 30);
    ht_insert(&ht, "cat", 11);                 /* same key -> update in place */
    printf("P2  ht_insert            = cat,dog,bird inserted; cat updated -> 11\n");

    int vcat = 0, vbird = 0, vmiss = 0;
    bool gcat = ht_get(&ht, "cat", &vcat);
    bool gbird = ht_get(&ht, "bird", &vbird);
    printf("P3  ht_get               = cat=%d(%s) bird=%d(%s)\n",
           vcat, gcat ? "hit" : "miss", vbird, gbird ? "hit" : "miss");

    bool ddog = ht_delete(&ht, "dog");
    bool gdog = ht_get(&ht, "dog", &vmiss);
    printf("P4  ht_delete            = del dog=%s, lookup dog now %s\n",
           ddog ? "ok" : "fail", gdog ? "hit" : "miss");

    /* ---- Min-heap: P5-P7 ---- */
    MinHeap heap = { .size = 0 };
    int vals[] = { 5, 3, 8, 1, 9, 2 };
    for (size_t i = 0; i < sizeof vals / sizeof vals[0]; i++)
        heap_push(&heap, vals[i]);
    printf("P5  heap_push            = pushed {5,3,8,1,9,2}, size=%d\n", heap.size);

    int peeked = 0;
    bool pk = heap_peek(&heap, &peeked);       /* capture min before draining */

    printf("P6  heap_pop_min         =");
    for (;;) {
        int mn = 0;
        if (!heap_pop_min(&heap, &mn)) break;
        printf(" %d", mn);
    }
    printf("  (ascending)\n");

    printf("P7  heap_peek            = min-before-drain=%d (%s)\n",
           peeked, pk ? "ok" : "empty");

    /* ---- Graph: P8-P11 ---- */
    Graph g = { .n = 6 };                       /* vertices 0..5; 5 is isolated */
    graph_add_edge(&g, 0, 1);
    graph_add_edge(&g, 0, 2);
    graph_add_edge(&g, 1, 3);
    graph_add_edge(&g, 2, 3);
    graph_add_edge(&g, 3, 4);
    printf("P8  graph_add_edge       = 6 vertices, edges 0-1,0-2,1-3,2-3,3-4\n");

    int border[MAXV];
    int bn = graph_bfs(&g, 0, border);
    printf("P9  graph_bfs(0)         =");
    for (int i = 0; i < bn; i++) printf(" %d", border[i]);
    printf("  (level order)\n");

    int dorder[MAXV];
    int dn = graph_dfs(&g, 0, dorder);
    printf("P10 graph_dfs(0)         =");
    for (int i = 0; i < dn; i++) printf(" %d", dorder[i]);
    printf("  (deep first)\n");

    int comps = graph_connected_components(&g);
    printf("P11 connected_components = %d (0-1-2-3-4 cluster + isolated 5)\n", comps);

    /* ---- Dynamic programming: P12-P14 ---- */
    int coins[] = { 1, 3, 4 };
    int mc = coin_change(coins, 3, 6);
    printf("P12 coin_change(6;1,3,4) = %d coins (3+3)\n", mc);

    int seq[] = { 10, 9, 2, 5, 3, 7, 101, 18 };
    int lis = lis_length(seq, 8);
    printf("P13 lis_length           = %d (e.g. 2,3,7,101)\n", lis);

    int ed = edit_distance("kitten", "sitting");
    printf("P14 edit_distance        = %d (kitten -> sitting)\n", ed);

    return 0;
}
