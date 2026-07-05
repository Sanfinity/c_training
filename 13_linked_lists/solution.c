/*
 * Topic 13: Linked Lists
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
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
    Node *prev = NULL;
    while (head) {
        Node *next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}

/* P4: middle node (fast/slow). */
Node *middle(Node *head) {
    Node *slow = head, *fast = head;
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    return slow;
}

/* P5: cycle detection (Floyd). */
bool has_cycle(Node *head) {
    Node *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next; fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

/* P7: n-th node from the end (one pass). */
Node *nth_from_end(Node *head, int n) {
    Node *lead = head;
    for (int i = 0; i < n; i++) { if (!lead) return NULL; lead = lead->next; }
    Node *trail = head;
    while (lead) { lead = lead->next; trail = trail->next; }
    return trail;
}

/* P8: merge two sorted lists (uses dummy head). */
Node *merge_sorted(Node *a, Node *b) {
    Node dummy; dummy.next = NULL;
    Node *tail = &dummy;
    while (a && b) {
        if (a->data <= b->data) { tail->next = a; a = a->next; }
        else                    { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    return dummy.next;
}

/* P11: remove duplicates from a sorted list. */
void remove_dups_sorted(Node *head) {
    for (Node *p = head; p && p->next; ) {
        if (p->data == p->next->data) {
            Node *dup = p->next;
            p->next = dup->next;
            free(dup);
        } else {
            p = p->next;
        }
    }
}

int main(void) {
    int a[] = {1,2,3,4,5};
    Node *list = from_array(a, 5);
    print_list("P1  build                -> ", list);

    list = reverse(list);
    print_list("P2  reverse              -> ", list);
    list = reverse(list);   /* back to 1..5 */

    printf("P4  middle               = %d\n", middle(list)->data);
    printf("P5  has_cycle            = %d\n", has_cycle(list));
    printf("P7  2nd_from_end         = %d\n", nth_from_end(list, 2)->data);

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
