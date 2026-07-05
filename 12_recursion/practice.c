/*
 * Topic 12: Recursion -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* P1: factorial. */
unsigned long factorial(unsigned n) {
    /* TODO: implement. HINT: base case n<=1 returns 1; otherwise n times factorial(n-1). */
    return 0; /* placeholder */
}

/* P2: fibonacci - memoized O(n). */
long fib_memo(int n, long *memo) {
    /* TODO: implement. HINT: return cached memo[n] if present; else store fib(n-1)+fib(n-2) before returning. */
    return 0; /* placeholder */
}

/* P3: sum of digits. */
int sum_digits(int n) {
    /* TODO: implement. HINT: work on |n|; add the last digit to the sum of the remaining digits. */
    return 0; /* placeholder */
}

/* P4: reverse a string in place recursively. */
void reverse_rec(char *s, int lo, int hi) {
    /* TODO: implement. HINT: swap the ends, then recurse inward until the indices meet. */
}

/* P5: fast exponentiation x^n, O(log n). */
long fast_pow(long x, unsigned n) {
    /* TODO: implement. HINT: compute half = pow(x, n/2); square it, multiplying by x once more when n is odd. */
    return 0; /* placeholder */
}

/* P6: gcd. */
int gcd(int a, int b) {
    /* TODO: implement. HINT: Euclid - when b is 0 the answer is a, else recurse on (b, a % b). */
    return 0; /* placeholder */
}

/* P7: recursive binary search; returns index or -1. */
int bsearch_rec(const int *a, int lo, int hi, int key) {
    /* TODO: implement. HINT: check the midpoint, then recurse into the half that could contain key. */
    return 0; /* placeholder */
}

/* P8: Tower of Hanoi - returns number of moves. */
int hanoi(int n, char from, char via, char to, int print) {
    /* TODO: implement. HINT: move n-1 aside, move disk n to its target, then move the n-1 back; sum the moves. */
    return 0; /* placeholder */
}

/* P11: permutations of a string (backtracking); counts them. */
int permute(char *s, int l, int r, int print) {
    /* TODO: implement. HINT: fix each character at position l via swaps, recurse on l+1, then swap back to backtrack. */
    return 0; /* placeholder */
}

/* P18: mutual recursion. */
bool is_even(unsigned n);
bool is_odd(unsigned n)  {
    /* TODO: implement. HINT: zero is not odd; otherwise defer to whether n-1 is even. */
    return false; /* placeholder */
}
bool is_even(unsigned n) {
    /* TODO: implement. HINT: zero is even; otherwise defer to whether n-1 is odd. */
    return false; /* placeholder */
}

int main(void) {
    printf("P1  factorial(6)         = %lu\n", factorial(6));

    long memo[20]; for (int i = 0; i < 20; i++) memo[i] = -1;
    printf("P2  fib(15)              = %ld\n", fib_memo(15, memo));

    printf("P3  sum_digits(9814)     = %d\n", sum_digits(9814));

    char s[] = "recursion";
    reverse_rec(s, 0, (int)strlen(s) - 1);
    printf("P4  reverse_rec          -> %s\n", s);

    printf("P5  fast_pow(2,10)       = %ld\n", fast_pow(2, 10));
    printf("P6  gcd(48,36)           = %d\n", gcd(48, 36));

    int arr[] = {1,3,5,7,9,11,13};
    printf("P7  bsearch(9)           = idx %d\n", bsearch_rec(arr, 0, 6, 9));

    printf("P8  hanoi(3) moves       = %d\n", hanoi(3, 'A', 'B', 'C', 0));

    char p[] = "abc";
    printf("P11 permutations of abc  = %d\n", permute(p, 0, 2, 0));

    printf("P18 is_even(10)=%d is_odd(7)=%d\n", is_even(10), is_odd(7));
    return 0;
}
