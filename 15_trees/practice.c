/*
 * Topic 15: Trees (Binary Trees & BST) -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct TNode {
    int data;
    struct TNode *left, *right;
} TNode;

TNode *new_node(int v) {
    TNode *n = malloc(sizeof *n);
    n->data = v; n->left = n->right = NULL;
    return n;
}

/* P6: BST insert. */
TNode *bst_insert(TNode *root, int v) {
    /* TODO: implement. HINT: an empty spot becomes a new_node; else recurse left/right by comparing v to root->data. */
    return NULL; /* placeholder */
}

/* P6: BST search. */
bool bst_search(const TNode *root, int v) {
    /* TODO: implement. HINT: compare v with root->data and follow the correct child until found or NULL. */
    return false; /* placeholder */
}

/* P2: traversals (print). */
void inorder(const TNode *r)  {
    /* TODO: implement. HINT: recurse left, print the node, recurse right (guard against NULL). */
}
void preorder(const TNode *r) {
    /* TODO: implement. HINT: print the node first, then recurse left and right (guard against NULL). */
}
void postorder(const TNode *r){
    /* TODO: implement. HINT: recurse left and right, then print the node (guard against NULL). */
}

/* P4: height. */
int height(const TNode *r) {
    /* TODO: implement. HINT: an empty tree has height 0; otherwise 1 plus the taller subtree's height. */
    return 0; /* placeholder */
}

/* P5: count nodes / leaves. */
int count_nodes(const TNode *r) {
    /* TODO: implement. HINT: 0 for NULL, else 1 plus the counts of both subtrees. */
    return 0; /* placeholder */
}
int count_leaves(const TNode *r) {
    /* TODO: implement. HINT: a node with no children counts as one leaf; sum leaves of both subtrees. */
    return 0; /* placeholder */
}

/* P7: validate BST via min/max bounds. */
static bool valid_bst(const TNode *r, long lo, long hi) {
    /* TODO: implement. HINT: each node must sit strictly within (lo, hi); tighten the bound as you recurse. */
    return false; /* placeholder */
}

/* P9: invert/mirror. */
void invert(TNode *r) {
    /* TODO: implement. HINT: swap the two children, then invert each subtree. */
}

/* P10: LCA in a BST. */
TNode *lca_bst(TNode *r, int a, int b) {
    /* TODO: implement. HINT: descend left/right while both a and b are on the same side; the split point is the LCA. */
    return NULL; /* placeholder */
}

/* P12: height-balanced check (returns height, -1 if unbalanced). */
static int balanced_h(const TNode *r) {
    /* TODO: implement. HINT: return subtree heights but propagate -1 once any subtree differs by more than 1. */
    return 0; /* placeholder */
}
bool is_balanced(const TNode *r) { return balanced_h(r) >= 0; }

void free_tree(TNode *r) { if (r) { free_tree(r->left); free_tree(r->right); free(r); } }

int main(void) {
    int vals[] = {50, 30, 70, 20, 40, 60, 80};
    TNode *root = NULL;
    for (int i = 0; i < 7; i++) root = bst_insert(root, vals[i]);

    printf("P2  inorder   -> "); inorder(root);  printf("\n");
    printf("P2  preorder  -> "); preorder(root); printf("\n");
    printf("P2  postorder -> "); postorder(root);printf("\n");
    printf("P4  height               = %d\n", height(root));
    printf("P5  nodes=%d leaves=%d\n", count_nodes(root), count_leaves(root));
    printf("P6  search(60)=%d search(65)=%d\n", bst_search(root,60), bst_search(root,65));
    printf("P7  valid_bst            = %d\n", valid_bst(root, -2147483648L, 2147483647L));
    TNode *lca = lca_bst(root, 20, 40);
    printf("P10 LCA(20,40)           = %d\n", lca ? lca->data : 0);
    printf("P12 is_balanced          = %d\n", is_balanced(root));

    invert(root);
    printf("P9  inverted inorder ->  "); inorder(root); printf("\n");

    free_tree(root);
    return 0;
}
