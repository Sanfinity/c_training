# 14 · Stacks & Queues

Fundamental linear structures. Interviewers ask you to implement them (array- and list-based),
plus the classic applications: expression evaluation, and building one from the other.
Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Implement a stack** (array-based) with push/pop/peek/isEmpty/isFull.
2. **Implement a stack** (linked-list based) — no fixed capacity.
3. **Implement a queue** (array/circular buffer) with enqueue/dequeue.
4. **Circular buffer / ring buffer** — the embedded favorite (producer/consumer).
5. **Balanced parentheses** using a stack.
6. **Evaluate a postfix (RPN) expression.**
7. **Reverse a string/array using a stack.**

## Frequently asked
8. **Implement a queue using two stacks.**
9. **Implement a stack using two queues.**
10. **Min-stack** — push/pop/getMin all in O(1).
11. **Next greater element** using a monotonic stack.
12. **Infix → postfix** conversion (shunting-yard).
13. **Deque** basics (double-ended queue).

## Good to know / tricky
14. **Why a circular buffer wastes one slot** (or uses a count) to distinguish full vs empty.
15. **Lock-free single-producer/single-consumer ring buffer** — why it works with head/tail indices.
16. **Stack overflow vs underflow** handling.
17. **Amortized O(1)** for the two-stack queue.

## Rapid-fire viva questions
- LIFO vs FIFO — which is which?
- How do you detect full vs empty in a circular buffer?
- Real embedded uses of a ring buffer? (UART RX/TX, audio, logging.)
- Why is a linked-list stack unbounded but slower per op than an array stack?
- How does a min-stack keep the min in O(1)?
