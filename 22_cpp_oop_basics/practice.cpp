/*
 * Topic 22: C++ OOP Basics -- PRACTICE TEMPLATE
 * Fill in each function/method body marked TODO. Answers are in solution.cpp.
 * Compile: g++ -std=c++17 -Wall -O2 practice.cpp -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <iostream>
#include <cstring>
#include <utility>

/* P3/P4/P8: a class that OWNS a heap buffer -> needs Rule of Three/Five.
 * Demonstrates deep copy and move semantics. */
class Buffer {
public:
    // P2: constructor with member-init list
    explicit Buffer(std::size_t n)                 // P11: explicit
        : size_(n), data_(new int[n]) {
        /* TODO: implement. HINT: zero-initialize all n elements of the freshly allocated buffer. */
    }

    // P3: copy constructor (DEEP copy)
    Buffer(const Buffer& other)
        : size_(other.size_), data_(new int[other.size_]) {
        /* TODO: implement. HINT: copy other's elements into this fresh buffer (a deep copy, not a pointer share). */
    }

    // P3: copy assignment (copy-and-swap for safety/self-assignment)
    Buffer& operator=(Buffer other) {              // by value -> copy made
        /* TODO: implement. HINT: swap this object's guts with the by-value copy, then return *this. */
        return *this; /* placeholder */
    }

    // P8: move constructor (steal the pointer)
    Buffer(Buffer&& other) noexcept
        : size_(0), data_(nullptr) {
        /* TODO: implement. HINT: take over other's size_ and data_, then leave other empty (null/0). */
    }

    // P3: destructor
    ~Buffer() {
        /* TODO: implement. HINT: release the heap buffer you allocated (match new[] with delete[]). */
    }

    friend void swap(Buffer& a, Buffer& b) noexcept { // P13: friend
        /* TODO: implement. HINT: exchange the size_ and data_ members of a and b. */
    }

    void set(std::size_t i, int v) { data_[i] = v; }
    // P9: const method
    int  get(std::size_t i) const {
        /* TODO: implement. HINT: return the element stored at index i (read-only accessor). */
        return 0; /* placeholder */
    }
    std::size_t size() const { return size_; }

private:
    std::size_t size_;
    int* data_;
};

/* P1/P5/P6/P7: encapsulated class with static member, chaining, operators. */
class Counter {
public:
    Counter() {
        /* TODO: implement. HINT: this class tracks how many objects exist; bump the shared tally here. */
    }
    Counter& add(int n) {                              // P5: return *this to chain
        /* TODO: implement. HINT: fold n into value_, then return *this so calls can chain. */
        return *this; /* placeholder */
    }
    int value() const { return value_; }

    bool operator==(const Counter& o) const {          // P7
        /* TODO: implement. HINT: two counters are equal when their value_ fields match. */
        return false; /* placeholder */
    }
    Counter operator+(const Counter& o) const {        // P7
        /* TODO: implement. HINT: build a Counter whose value_ is the sum of the two operands' values. */
        return {}; /* placeholder */
    }
    static int instances() {                           // P6: static method
        /* TODO: implement. HINT: report the shared instance tally. */
        return 0; /* placeholder */
    }

private:
    int value_ = 0;                                    // P12: in-class init
    static int instances_;                             // P6: static member
};
int Counter::instances_ = 0;                           // definition

// P7: operator<< as a free function
std::ostream& operator<<(std::ostream& os, const Counter& c) {
    /* TODO: implement. HINT: stream the counter's value() into os, then return os for chaining. */
    return os; /* placeholder */
}

int main() {
    // P3/P4: deep copy
    Buffer a(3);
    a.set(0, 10); a.set(1, 20); a.set(2, 30);
    Buffer b = a;                 // copy ctor
    b.set(0, 999);                // must not affect 'a'
    std::cout << "P4  deep copy: a[0]=" << a.get(0)
              << " b[0]=" << b.get(0) << " (independent)\n";

    // P8: move
    Buffer c = std::move(a);      // move ctor; 'a' now empty
    std::cout << "P8  move: c.size=" << c.size()
              << " a.size=" << a.size() << " (moved-from)\n";

    // P5: method chaining
    Counter cnt;
    cnt.add(5).add(10).add(2);
    std::cout << "P5  chaining -> value=" << cnt << "\n";

    // P7: operators
    Counter x; x.add(3);
    Counter y; y.add(3);
    Counter sum = cnt + x;
    std::cout << "P7  operator+ -> " << sum
              << ", operator== (x==y)=" << (x == y) << "\n";

    // P6: static instance count
    std::cout << "P6  Counter instances = " << Counter::instances() << "\n";
    return 0;
}
