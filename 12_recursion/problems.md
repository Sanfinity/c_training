# 12 · Recursion

Recursion tests whether you can reason about base cases, the call stack, and converting
between recursive and iterative forms. Common in phone screens. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Factorial** (recursive and iterative).
2. **Fibonacci** — naive recursion, then memoized/iterative; discuss O(2^n) vs O(n).
3. **Sum of digits** of an integer.
4. **Reverse a string** recursively.
5. **Power `x^n`** in O(log n) (fast exponentiation).
6. **GCD** (Euclid's algorithm).
7. **Binary search**, recursive.
8. **Tower of Hanoi** — count/print the moves.

## Frequently asked
9. **Print numbers 1..n / n..1** recursively (understand pre vs post order of work).
10. **Check if a string/array is a palindrome** recursively.
11. **Generate all permutations** of a string/array (backtracking).
12. **Generate all subsets** (power set).
13. **Flatten/traverse** a nested structure recursively.
14. **Ackermann function** — why it's a classic (non-primitive-recursive).

## Good to know / tricky
15. **Tail recursion** — what it is and how a compiler can turn it into a loop.
16. **Stack overflow from deep recursion** — the embedded concern; convert to iterative.
17. **Recursion vs iteration** trade-offs (clarity vs stack usage/perf).
18. **Mutual recursion** (is_even/is_odd).

## Rapid-fire viva questions
- What are the two essential parts of any recursion? (base case + progress toward it.)
- Why is naive Fibonacci exponential?
- What is tail recursion and why does it matter on limited stacks?
- How deep can recursion go before a stack overflow, roughly?
- Convert a simple recursion (factorial) to iteration on the spot.
