/*
 * Topic 15: Trees (Binary Trees & BST)
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
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
    if (!root) return new_node(v);
    if (v < root->data) root->left  = bst_insert(root->left, v);
    else                 root->right = bst_insert(root->right, v);
    return root;
}

/* P6: BST search. */
bool bst_search(const TNode *root, int v) {
    if (!root) return false;
    if (root->data == v) return true;
    return v < root->data ? bst_search(root->left, v) : bst_search(root->right, v);
}

/* P2: traversals (print). */
void inorder(const TNode *r)  { if (r) { inorder(r->left);  printf("%d ", r->data); inorder(r->right); } }
void preorder(const TNode *r) { if (r) { printf("%d ", r->data); preorder(r->left); preorder(r->right); } }
void postorder(const TNode *r){ if (r) { postorder(r->left); postorder(r->right); printf("%d ", r->data); } }

/* P4: height. */
int height(const TNode *r) {
    if (!r) return 0;
    int l = height(r->left), rt = height(r->right);
    return 1 + (l > rt ? l : rt);
}

/* P5: count nodes / leaves. */
int count_nodes(const TNode *r) { return r ? 1 + count_nodes(r->left) + count_nodes(r->right) : 0; }
int count_leaves(const TNode *r) {
    if (!r) return 0;
    if (!r->left && !r->right) return 1;
    return count_leaves(r->left) + count_leaves(r->right);
}

/* P7: validate BST via min/max bounds. */
static bool valid_bst(const TNode *r, long lo, long hi) {
    if (!r) return true;
    if (r->data <= lo || r->data >= hi) return false;
    return valid_bst(r->left, lo, r->data) && valid_bst(r->right, r->data, hi);
}

/* P9: invert/mirror. */
void invert(TNode *r) {
    if (!r) return;
    TNode *t = r->left; r->left = r->right; r->right = t;
    invert(r->left); invert(r->right);
}

/* P10: LCA in a BST. */
TNode *lca_bst(TNode *r, int a, int b) {
    while (r) {
        if (a < r->data && b < r->data) r = r->left;
        else if (a > r->data && b > r->data) r = r->right;
        else return r;
    }
    return NULL;
}

/* P12: height-balanced check (returns height, -1 if unbalanced). */
static int balanced_h(const TNode *r) {
    if (!r) return 0;
    int l = balanced_h(r->left);  if (l < 0) return -1;
    int rt = balanced_h(r->right); if (rt < 0) return -1;
    if ((l > rt ? l - rt : rt - l) > 1) return -1;
    return 1 + (l > rt ? l : rt);
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
    printf("P10 LCA(20,40)           = %d\n", lca_bst(root, 20, 40)->data);
    printf("P12 is_balanced          = %d\n", is_balanced(root));

    invert(root);
    printf("P9  inverted inorder ->  "); inorder(root); printf("\n");

    free_tree(root);
    return 0;
}
