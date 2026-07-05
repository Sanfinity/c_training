/*
 * Topic 34: Data Structures & Algorithms: Hashing, Heaps, Graphs, DP -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
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
    /* TODO: implement. HINT: seed h=5381, then for each byte fold it in with
     * h = h*33 + c ((h<<5)+h+c); return the accumulator. */
    return 0; /* placeholder */
}

/* P2: insert / update (open addressing, linear probing).
 * Start at the home slot hash%cap and step forward on collision. Update in
 * place if the key already exists, otherwise drop into the first EMPTY or
 * DELETED slot seen. Average O(1); it degrades as the LOAD FACTOR n/cap rises
 * (real tables rehash to a bigger array past ~0.7). Returns false if full. */
bool ht_insert(HashTable *t, const char *key, int value) {
    /* TODO: implement. HINT: home = hash % cap; linear-probe forward; update if
     * key matches, else drop into the first EMPTY/DELETED slot; refuse when full. */
    return false; /* placeholder */
}

/* P3: lookup. Probe from the home slot; a match writes *out and returns true.
 * Stop at the first EMPTY slot -- the key cannot lie beyond it. Average O(1),
 * worst O(cap) under heavy clustering. */
bool ht_get(const HashTable *t, const char *key, int *out) {
    /* TODO: implement. HINT: probe from hash % cap; stop/return false at the first
     * EMPTY slot; on a USED slot whose key matches, write *out and return true. */
    return false; /* placeholder */
}

/* P4: delete. Mark the slot DELETED (a tombstone) rather than EMPTY, so probe
 * chains that ran THROUGH this slot are not broken for other keys. */
bool ht_delete(HashTable *t, const char *key) {
    /* TODO: implement. HINT: probe like ht_get; on a match set the slot state to
     * DELETED (a tombstone), not EMPTY, so later probes still pass through it. */
    return false; /* placeholder */
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
    /* TODO: implement. HINT: append at the end, then sift up: while smaller than
     * its parent (index (i-1)/2), swap upward. */
    return false; /* placeholder */
}

/* P6: pop-min -> sift-down. The root (index 0) is the minimum. Copy it out,
 * move the last element to the root, shrink, then sift down toward the smaller
 * child until the heap property is restored. O(log n). */
bool heap_pop_min(MinHeap *h, int *out) {
    /* TODO: implement. HINT: save data[0] to *out, move the last element to the
     * root, shrink, then sift down toward the smaller child. */
    return false; /* placeholder */
}

/* P7: peek. The minimum is always at index 0 -- read it without removing. O(1). */
bool heap_peek(const MinHeap *h, int *out) {
    /* TODO: implement. HINT: the min of a min-heap is always at index 0; copy it
     * to *out (return false if the heap is empty). */
    return false; /* placeholder */
}

/* ===================================================================== */
/* GRAPH as a fixed-size adjacency list: adj[u] holds u's neighbours,    */
/* deg[u] how many. Space O(V+E) (a matrix would be O(V^2)).             */
/* ===================================================================== */
#define MAXV 16
typedef struct { int adj[MAXV][MAXV]; int deg[MAXV]; int n; } Graph;

/* P8: add an UNDIRECTED edge -> append each endpoint to the other's list. */
bool graph_add_edge(Graph *g, int u, int v) {
    /* TODO: implement. HINT: bounds-check u,v; for an undirected edge append v to
     * u's list and u to v's list, bumping each degree counter. */
    return false; /* placeholder */
}

/* P9: BFS -> visit order via a FIFO queue. Explores level by level, so it
 * finds the fewest-hops (shortest) path in an unweighted graph. Fills order[]
 * and returns the count. O(V+E). */
int graph_bfs(const Graph *g, int start, int *order) {
    /* TODO: implement. HINT: use a FIFO queue and a seen[] array; enqueue start,
     * then repeatedly dequeue, record it, and enqueue its unseen neighbours. */
    return 0; /* placeholder */
}

/* P10: DFS -> visit order via an EXPLICIT stack (avoids recursion depth
 * limits). Goes as deep as possible before backtracking; the basis for cycle
 * detection and topological sort. Fills order[] and returns the count. O(V+E). */
int graph_dfs(const Graph *g, int start, int *order) {
    /* TODO: implement. HINT: use an explicit stack (or recursion) and a seen[]
     * array; pop, skip if seen, record it, push unseen neighbours. */
    return 0; /* placeholder */
}

/* P11: count CONNECTED COMPONENTS. Flood (BFS) from every not-yet-seen vertex;
 * each fresh flood marks exactly one component. O(V+E). */
int graph_connected_components(const Graph *g) {
    /* TODO: implement. HINT: loop over all vertices; each time you hit an
     * unvisited one, count +1 and BFS/DFS-flood everything reachable from it. */
    return 0; /* placeholder */
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
    /* TODO: implement. HINT: bottom-up dp[a] = min over coins of dp[a-coin]+1,
     * dp[0]=0; use amount+1 as "infinity"; return -1 if unreachable. */
    return 0; /* placeholder */
}

/* P13: longest increasing subsequence (LIS) length.
 * dp[i] = longest increasing run ending at i = 1 + max(dp[j]) for j<i with
 * a[j] < a[i]. Answer is the largest dp[i]. Time O(n^2), space O(n).
 * (A heap/binary-search variant reaches O(n log n).) */
int lis_length(const int *a, int n) {
    /* TODO: implement. HINT: dp[i] = 1 + max(dp[j]) over j<i with a[j] < a[i];
     * the answer is the largest dp[i]. */
    return 0; /* placeholder */
}

/* P14: edit distance (Levenshtein) -> min insert/delete/substitute operations
 * to turn a into b. dp[i][j] works over prefixes: equal last chars cost
 * nothing (dp[i-1][j-1]); otherwise 1 + min(delete, insert, substitute).
 * Time and space O(len(a) * len(b)). */
int edit_distance(const char *a, const char *b) {
    /* TODO: implement. HINT: dp[i][j] over prefixes; if the last chars match take
     * dp[i-1][j-1], else 1 + min(insert, delete, substitute). */
    return 0; /* placeholder */
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

    int border[MAXV] = { 0 };
    int bn = graph_bfs(&g, 0, border);
    printf("P9  graph_bfs(0)         =");
    for (int i = 0; i < bn; i++) printf(" %d", border[i]);
    printf("  (level order)\n");

    int dorder[MAXV] = { 0 };
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
