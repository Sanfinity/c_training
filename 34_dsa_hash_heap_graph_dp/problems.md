# 34 · Data Structures & Algorithms: Hashing, Heaps, Graphs, DP

The four workhorse tools of coding interviews — a **hash table** for near-O(1) key/value lookup, a **binary heap** for priorities, **graph** traversals (BFS/DFS), and **dynamic programming** — each built from scratch on fixed-size, leak-free storage in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **String hashing (djb2)** — a hash function maps a variable-length key to a bucket index by accumulating `h = h*33 + c` over the bytes (seed 5381). A good hash spreads keys uniformly so few *collide* (land on the same slot); hashing a string is O(its length). → djb2_hash (P1)
2. **Hash-table insert (open addressing)** — compute `hash % capacity`, then linear-probe forward past occupied slots to the first free one (or update in place on a key match). Cost is ~O(1) until the **load factor** α = entries/capacity climbs; past ~0.7 probe runs lengthen and real tables rehash into a bigger array. → ht_insert (P2)
3. **Hash-table lookup** — probe from the home slot and stop at the first EMPTY slot (the key cannot lie beyond it). Average O(1), worst O(n) when clustering or a full table forces long probe runs. → ht_get (P3)
4. **Hash-table delete (tombstones)** — you cannot blank a slot to EMPTY on delete or you would sever the probe chains of other keys; mark it DELETED (a *tombstone*) that lookups skip over and later inserts may reuse. → ht_delete (P4)
5. **Min-heap push (sift-up)** — a binary heap is a complete tree packed into an array (node `i`'s parent is `(i-1)/2`, children `2i+1`/`2i+2`). Append at the end and swap upward while smaller than the parent; O(log n). → heap_push (P5)
6. **Min-heap pop-min (sift-down)** — the minimum sits at the root (index 0); copy it out, overwrite the root with the last element, shrink, then sift down toward the smaller child until the heap property holds again; O(log n). → heap_pop_min (P6)
7. **Heap peek** — reading the current minimum is O(1) because it is always the root at index 0 (no removal, no restructuring). → heap_peek (P7)

## Frequently asked
8. **Graph adjacency list** — store, per vertex, the list of its neighbours; an undirected edge appends each endpoint to the other's list. Space O(V+E) versus O(V²) for an adjacency matrix — lists win decisively on sparse graphs. → graph_add_edge (P8)
9. **Breadth-first search (BFS)** — a FIFO **queue** drives a level-by-level sweep, so BFS finds the fewest-edges (shortest) path in an *unweighted* graph; O(V+E) time and O(V) memory for the queue and visited set. → graph_bfs (P9)
10. **Depth-first search (DFS)** — a **stack** (explicit here, or the call stack via recursion) drives a go-deep-then-backtrack sweep; it underlies cycle detection, topological sort, and path finding; O(V+E). → graph_dfs (P10)
11. **Connected components** — launch a fresh BFS/DFS flood from every still-unvisited vertex; the number of floods equals the number of components; O(V+E). → graph_connected_components (P11)

## Good to know / tricky
12. **Coin change (min coins)** — bottom-up DP: `dp[a] = min over coins of dp[a − coin] + 1`, with `dp[0] = 0`. It fits DP because it has *optimal substructure* and *overlapping subproblems*; note that greedy "biggest coin first" fails for coins {1,3,4} making 6 (greedy 4+1+1 = 3, DP 3+3 = 2). Time O(amount × #coins), space O(amount). → coin_change (P12)
13. **Longest increasing subsequence (LIS)** — `dp[i]` = length of the longest increasing run ending at `i` = `1 + max(dp[j])` over `j < i` with `a[j] < a[i]`; the answer is the largest `dp[i]`. This DP is O(n²) time / O(n) space; a patience-sorting + binary-search variant reaches O(n log n). → lis_length (P13)
14. **Edit distance (Levenshtein)** — `dp[i][j]` = operations to turn `a[0..i)` into `b[0..j)`: if the last characters match take `dp[i−1][j−1]`, else `1 + min(insert, delete, substitute)`. Time and space O(m·n) (space reducible to O(min(m,n)) with a rolling row). → edit_distance (P14)
15. **Collision resolution & rehashing** — the two families are *separate chaining* (a linked list/bucket per slot) and *open addressing* (probe sequences: linear, quadratic, or double hashing). Both degrade as the load factor rises, so tables resize and rehash (commonly past α≈0.7); linear probing is cache-friendly but suffers *primary clustering*.
16. **Heap vs balanced BST** — a heap gives O(1) find-min and O(log n) push/pop but *no* ordered traversal and O(n) search; a balanced BST gives O(log n) search/insert/delete plus in-order iteration and min/max. Use a heap for a priority queue, a BST/`std::map` when you also need ordering or arbitrary lookup.
17. **BFS vs DFS — when to use which** — BFS for shortest hops, level-order, or a wide-but-shallow search (costs O(V) queue memory); DFS for path existence, cycle detection, topological/SCC work, or deep graphs where its stack is cheaper. Both run in O(V+E).
18. **When DP applies** — the problem needs *optimal substructure* (an optimal answer is composed of optimal sub-answers) and *overlapping subproblems* (the same sub-answers recur); then memoize (top-down) or tabulate (bottom-up). Without overlap, plain divide-and-conquer suffices; when a locally optimal choice is provably globally optimal, greedy beats DP.

## Rapid-fire viva questions
- What is a load factor, and what happens to probe length and lookup time as it approaches 1.0?
- Why must an open-addressing table use tombstones instead of blanking a slot on delete?
- Give the average and worst-case Big-O of hash-table insert, get, and delete.
- For a heap node at index `i`, what are the indices of its parent, left child, and right child?
- Why can a binary heap live in a flat array with no child pointers?
- Heap vs balanced BST — name one operation each does strictly better.
- Which traversal finds the shortest path in an unweighted graph, and what data structure powers it?
- Adjacency list vs matrix — which for a sparse graph, and what are their space costs?
- Name the two properties a problem must have before DP is the right tool.
- Memoization vs tabulation — which is top-down and which is bottom-up?
- Why does greedy "largest coin first" fail for coins {1,3,4} making 6, and what does DP return instead?
- State the time and space complexity of the edit-distance DP.
