# 15 · Trees (Binary Trees & BST)

Trees show up in most on-site loops. Know traversals cold (recursive and iterative), plus
BST operations and the common height/balance/LCA questions. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Build a binary tree** and free it (post-order).
2. **In-order, pre-order, post-order traversal** (recursive).
3. **Level-order traversal (BFS)** using a queue.
4. **Height / depth** of a tree.
5. **Count nodes / count leaves.**
6. **BST insert and search.**
7. **Validate a BST** (in-order is sorted / min-max bounds).
8. **Find min/max** in a BST.

## Frequently asked
9. **Mirror / invert** a binary tree.
10. **Lowest common ancestor** (BST version and general version).
11. **Diameter** of a binary tree.
12. **Check if a tree is height-balanced.**
13. **Iterative in-order traversal** using an explicit stack.
14. **Level with maximum sum / print level by level.**

## Good to know / tricky
15. **Serialize / deserialize** a tree.
16. **k-th smallest** in a BST (in-order counting).
17. **Convert a sorted array to a balanced BST.**
18. **Morris traversal** (O(1) space) — advanced.

## Rapid-fire viva questions
- In-order of a BST yields what? (sorted order.)
- Time complexity of BST search? Worst case? (O(h); O(n) if degenerate.)
- How does level-order use a queue?
- What makes a tree balanced?
- Difference between complete, full, and perfect binary trees?
