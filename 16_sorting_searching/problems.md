# 16 · Sorting & Searching

Know the classic sorts (implementation, time/space, stability) and binary search plus its
variants. You will be asked to code at least one sort and binary search by hand. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Bubble sort** — and its best-case early-exit optimization.
2. **Selection sort.**
3. **Insertion sort** — why it's great for nearly-sorted / small inputs.
4. **Merge sort** — stable, O(n log n), O(n) space.
5. **Quick sort** — partition scheme, average O(n log n), worst O(n²).
6. **Binary search** (iterative) on a sorted array.
7. **First / last occurrence** of a target (lower_bound / upper_bound).

## Frequently asked
8. **Counting sort** (non-comparison, for small integer ranges).
9. **Find the k-th smallest/largest** (quickselect).
10. **Search in a rotated sorted array.**
11. **Square root via binary search** (integer sqrt).
12. **Stability**: which sorts are stable, and why it matters.
13. **Complexity table** for all sorts (time best/avg/worst, space, stable?).

## Good to know / tricky
14. **Why quicksort's worst case is O(n²)** and how randomized/median-of-three pivots help.
15. **In-place vs out-of-place**; which sorts are in-place.
16. **When to prefer which sort** on embedded (memory-limited → heapsort/insertion).
17. **Binary search overflow bug**: `mid = (lo+hi)/2` vs `lo+(hi-lo)/2`.

## Rapid-fire viva questions
- Which common sorts are stable? (merge, insertion, bubble, counting.)
- Best/avg/worst of quicksort and mergesort?
- Why is `lo + (hi - lo)/2` preferred?
- Which sort would you use for a nearly-sorted array? (insertion.)
- How does binary search need the data to be arranged? (sorted.)
- Time complexity of quickselect on average? (O(n).)
