/*
 * Topic 12: Recursion
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* P1: factorial. */
unsigned long factorial(unsigned n) {
    return (n <= 1) ? 1UL : n * factorial(n - 1);
}

/* P2: fibonacci - memoized O(n). */
long fib_memo(int n, long *memo) {
    if (n < 2) return n;
    if (memo[n] != -1) return memo[n];
    return memo[n] = fib_memo(n - 1, memo) + fib_memo(n - 2, memo);
}

/* P3: sum of digits. */
int sum_digits(int n) {
    n = n < 0 ? -n : n;
    return (n < 10) ? n : (n % 10) + sum_digits(n / 10);
}

/* P4: reverse a string in place recursively. */
void reverse_rec(char *s, int lo, int hi) {
    if (lo >= hi) return;
    char t = s[lo]; s[lo] = s[hi]; s[hi] = t;
    reverse_rec(s, lo + 1, hi - 1);
}

/* P5: fast exponentiation x^n, O(log n). */
long fast_pow(long x, unsigned n) {
    if (n == 0) return 1;
    long half = fast_pow(x, n / 2);
    return (n & 1) ? half * half * x : half * half;
}

/* P6: gcd. */
int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }

/* P7: recursive binary search; returns index or -1. */
int bsearch_rec(const int *a, int lo, int hi, int key) {
    if (lo > hi) return -1;
    int mid = lo + (hi - lo) / 2;
    if (a[mid] == key) return mid;
    return (a[mid] < key) ? bsearch_rec(a, mid + 1, hi, key)
                          : bsearch_rec(a, lo, mid - 1, key);
}

/* P8: Tower of Hanoi - returns number of moves. */
int hanoi(int n, char from, char via, char to, int print) {
    if (n == 0) return 0;
    int moves = hanoi(n - 1, from, to, via, print);
    if (print) printf("      move disk %d: %c -> %c\n", n, from, to);
    moves += 1;
    moves += hanoi(n - 1, via, from, to, print);
    return moves;
}

/* P11: permutations of a string (backtracking); counts them. */
int permute(char *s, int l, int r, int print) {
    if (l == r) { if (print) printf("      %s\n", s); return 1; }
    int count = 0;
    for (int i = l; i <= r; i++) {
        char t = s[l]; s[l] = s[i]; s[i] = t;
        count += permute(s, l + 1, r, print);
        t = s[l]; s[l] = s[i]; s[i] = t;   /* backtrack */
    }
    return count;
}

/* P18: mutual recursion. */
bool is_even(unsigned n);
bool is_odd(unsigned n)  { return n == 0 ? false : is_even(n - 1); }
bool is_even(unsigned n) { return n == 0 ? true  : is_odd(n - 1); }

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
