# 03 · Arrays (1-D & 2-D)

Array questions test index math, in-place algorithms, and understanding that arrays are
contiguous memory. Many "DSA" warm-ups live here. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Find the max and min** in one pass.
2. **Find the second largest** element (handle duplicates).
3. **Reverse an array in place.**
4. **Left-rotate an array by k** positions in O(n) time, O(1) space (reversal algorithm).
5. **Remove duplicates from a sorted array** in place; return the new length.
6. **Move all zeros to the end** while keeping order (two pointers).
7. **Find the missing number** in 1..n (sum or XOR).
8. **Kadane's algorithm** — maximum contiguous subarray sum.

## Frequently asked
9. **Two-sum**: find a pair that sums to a target (sorted → two pointers).
10. **Merge two sorted arrays** into one sorted array.
11. **Find the majority element** (Boyer–Moore voting).
12. **Rotate a 2-D matrix 90°** in place.
13. **Row/column sums of a 2-D array**; explain row-major memory layout.
14. **Leaders in an array** (every element greater than all to its right).

## Good to know / tricky
15. **Dutch National Flag** — sort an array of 0s, 1s, 2s in one pass.
16. **Find duplicate** in [1..n] with one repeat (Floyd / sum).
17. **Prefix-sum** array and answering range-sum queries in O(1).
18. Why does a 2-D array `a[R][C]` decay to `int (*)[C]`, not `int **`?

## Rapid-fire viva questions
- What is the difference between `int a[10]` and `int *a`?
- How is a 2-D array laid out in memory (row-major)? Address of `a[i][j]`?
- Why is `sizeof(arr)/sizeof(arr[0])` the idiom for length, and when does it break?
- Can you pass a fixed-size array to a function "by value"? (No — it decays.)
- What is a VLA and what are its risks on embedded/stack-limited targets?
