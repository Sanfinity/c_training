/*
 * Topic 13: Linked Lists -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

/* P1: helpers - push front, build from array, print, free. */
Node *push(Node *head, int v) {
    Node *n = malloc(sizeof *n);
    n->data = v; n->next = head;
    return n;
}
Node *from_array(const int *a, int n) {
    Node *head = NULL;
    for (int i = n - 1; i >= 0; i--) head = push(head, a[i]);
    return head;
}
void print_list(const char *label, const Node *h) {
    printf("%s", label);
    for (const Node *p = h; p; p = p->next) printf("%d ", p->data);
    printf("\n");
}
void free_list(Node *h) {
    while (h) { Node *t = h; h = h->next; free(t); }
}

/* P2: iterative reverse. */
Node *reverse(Node *head) {
    /* TODO: implement. HINT: walk the list carrying prev/curr/next pointers and flip each link. */
    return NULL; /* placeholder */
}

/* P4: middle node (fast/slow). */
Node *middle(Node *head) {
    /* TODO: implement. HINT: advance a slow pointer by one and a fast pointer by two each step. */
    return NULL; /* placeholder */
}

/* P5: cycle detection (Floyd). */
bool has_cycle(Node *head) {
    /* TODO: implement. HINT: slow/fast pointers meet if a cycle exists (tortoise and hare). */
    return false; /* placeholder */
}

/* P7: n-th node from the end (one pass). */
Node *nth_from_end(Node *head, int n) {
    /* TODO: implement. HINT: send a lead pointer n nodes ahead, then move both until lead falls off. */
    return NULL; /* placeholder */
}

/* P8: merge two sorted lists (uses dummy head). */
Node *merge_sorted(Node *a, Node *b) {
    /* TODO: implement. HINT: use a dummy head and repeatedly splice the smaller of a/b onto the tail. */
    return NULL; /* placeholder */
}

/* P11: remove duplicates from a sorted list. */
void remove_dups_sorted(Node *head) {
    /* TODO: implement. HINT: when a node equals its successor, unlink and free the successor. */
}

int main(void) {
    int a[] = {1,2,3,4,5};
    Node *list = from_array(a, 5);
    print_list("P1  build                -> ", list);

    list = reverse(list);
    print_list("P2  reverse              -> ", list);
    list = reverse(list);   /* back to 1..5 */

    Node *mid = middle(list);
    printf("P4  middle               = %d\n", mid ? mid->data : 0);
    printf("P5  has_cycle            = %d\n", has_cycle(list));
    Node *nth = nth_from_end(list, 2);
    printf("P7  2nd_from_end         = %d\n", nth ? nth->data : 0);

    int x[] = {1,3,5}, y[] = {2,4,6};
    Node *m = merge_sorted(from_array(x,3), from_array(y,3));
    print_list("P8  merge_sorted         -> ", m);
    free_list(m);

    int d[] = {1,1,2,3,3,3,4};
    Node *dl = from_array(d, 7);
    remove_dups_sorted(dl);
    print_list("P11 remove_dups          -> ", dl);
    free_list(dl);

    free_list(list);
    return 0;
}
