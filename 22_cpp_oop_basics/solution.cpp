/*
 * Topic 22: C++ OOP Basics
 * Compile: g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o solution && ./solution
 *
 * Each problem is illustrated by a small class; main() demonstrates all of them.
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
        for (std::size_t i = 0; i < n; ++i) data_[i] = 0;
    }

    // P3: copy constructor (DEEP copy)
    Buffer(const Buffer& other)
        : size_(other.size_), data_(new int[other.size_]) {
        std::memcpy(data_, other.data_, size_ * sizeof(int));
    }

    // P3: copy assignment (copy-and-swap for safety/self-assignment)
    Buffer& operator=(Buffer other) {              // by value -> copy made
        swap(*this, other);
        return *this;
    }

    // P8: move constructor (steal the pointer)
    Buffer(Buffer&& other) noexcept
        : size_(other.size_), data_(other.data_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // P3: destructor
    ~Buffer() { delete[] data_; }

    friend void swap(Buffer& a, Buffer& b) noexcept { // P13: friend
        std::swap(a.size_, b.size_);
        std::swap(a.data_, b.data_);
    }

    void set(std::size_t i, int v) { data_[i] = v; }
    int  get(std::size_t i) const  { return data_[i]; } // P9: const method
    std::size_t size() const { return size_; }

private:
    std::size_t size_;
    int* data_;
};

/* P1/P5/P6/P7: encapsulated class with static member, chaining, operators. */
class Counter {
public:
    Counter() { ++instances_; }
    Counter& add(int n) { value_ += n; return *this; } // P5: return *this to chain
    int value() const { return value_; }

    bool operator==(const Counter& o) const { return value_ == o.value_; } // P7
    Counter operator+(const Counter& o) const {
        Counter r; r.value_ = value_ + o.value_; return r;
    }
    static int instances() { return instances_; }      // P6: static method

private:
    int value_ = 0;                                    // P12: in-class init
    static int instances_;                             // P6: static member
};
int Counter::instances_ = 0;                           // definition

// P7: operator<< as a free function
std::ostream& operator<<(std::ostream& os, const Counter& c) {
    return os << c.value();
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
