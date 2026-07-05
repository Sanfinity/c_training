# 13 · Linked Lists

The single most common data-structure interview topic. Singly & doubly linked lists,
pointer surgery, and the fast/slow pointer patterns. Expect at least one of these live.
Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Build a list, insert at head/tail, print, and free it** (no leaks).
2. **Reverse a singly linked list** iteratively (the #1 asked question).
3. **Reverse recursively.**
4. **Find the middle** node (fast/slow pointers).
5. **Detect a cycle** (Floyd's tortoise & hare).
6. **Find the start of the cycle.**
7. **Find the n-th node from the end** (two pointers, one pass).
8. **Merge two sorted lists.**

## Frequently asked
9. **Remove the n-th node from the end.**
10. **Delete a node given only a pointer to it** (copy-next trick).
11. **Detect and remove duplicates** (sorted list).
12. **Check if a list is a palindrome.**
13. **Add two numbers represented as lists.**
14. **Doubly linked list**: insert/delete both directions.

## Good to know / tricky
15. **Intersection point** of two lists.
16. **Rotate a list by k.**
17. **Why a dummy/sentinel head** simplifies edge cases.
18. **Reverse in groups of k.**

## Rapid-fire viva questions
- Time/space complexity of reversing a list? (O(n)/O(1).)
- How do fast/slow pointers find the middle and detect cycles?
- Array vs linked list — when do you pick each?
- Why keep a `**` (pointer to pointer) when deleting/inserting?
- What's the danger of losing the `next` pointer during reversal?
