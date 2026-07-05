/*
 * Topic 25: C++ Templates, STL & Move Semantics -- PRACTICE TEMPLATE
 * Fill in each function/method body marked TODO. Answers are in solution.cpp.
 * Compile: g++ -std=c++17 -Wall -O2 practice.cpp -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>

/* P1: function template. */
template <typename T>
T my_max(const T& a, const T& b) {
    /* TODO: implement. HINT: return whichever of a or b compares greater. */
    return {}; /* placeholder */
}

/* P2: class template - a tiny generic stack. */
template <typename T>
class Stack {
public:
    void push(const T& v) {
        /* TODO: implement. HINT: append v onto the underlying vector. */
    }
    T pop() {
        /* TODO: implement. HINT: read the back element, pop it off, and return it. */
        return {}; /* placeholder */
    }
    bool empty() const {
        /* TODO: implement. HINT: true when the underlying vector holds nothing. */
        return false; /* placeholder */
    }
    std::size_t size() const {
        /* TODO: implement. HINT: report how many elements are stored. */
        return 0; /* placeholder */
    }
private:
    std::vector<T> data_;
};

/* P3: template specialization - customize for bool. */
template <typename T>
const char* type_tag(const T&) {
    /* TODO: implement. HINT: return the generic tag for any non-specialized type. */
    return ""; /* placeholder */
}
template <>
const char* type_tag<bool>(const bool&) {
    /* TODO: implement. HINT: this bool specialization should return its own distinct tag. */
    return ""; /* placeholder */
}

/* P7/P8: a type that reports copy vs move so we can SEE move semantics. */
class Tracked {
public:
    Tracked() : id_(++counter_) {}
    Tracked(const Tracked&) {
        /* TODO: implement. HINT: this is the COPY constructor; announce that a copy happened. */
    }
    Tracked(Tracked&&) noexcept {
        /* TODO: implement. HINT: this is the MOVE constructor; announce that a move happened. */
    }
    Tracked& operator=(const Tracked&) {
        /* TODO: implement. HINT: copy-assignment; announce it, then return *this. */
        return *this; /* placeholder */
    }
    Tracked& operator=(Tracked&&) noexcept {
        /* TODO: implement. HINT: move-assignment; announce it, then return *this. */
        return *this; /* placeholder */
    }
private:
    int id_;
    static int counter_;
};
int Tracked::counter_ = 0;

/* P11: variadic template - type-safe print. */
template <typename... Args>
void print_all(const Args&... args) {
    /* TODO: implement. HINT: use a C++17 fold expression to stream each arg separated by spaces, then newline. */
}

/* P9: perfect forwarding. */
template <typename T>
Tracked forward_into(T&& x) {
    /* TODO: implement. HINT: build a Tracked from std::forward<T>(x) so lvalue/rvalue-ness is preserved. */
    return {}; /* placeholder */
}

int main() {
    // P1
    std::cout << "P1  my_max(3,9)=" << my_max(3, 9)
              << " my_max(2.5,1.5)=" << my_max(2.5, 1.5) << "\n";

    // P2
    Stack<std::string> st;
    st.push("a"); st.push("b"); st.push("c");
    std::cout << "P2  Stack<string> pop=" << st.pop()
              << " size=" << st.size() << "\n";

    // P3
    std::cout << "P3  type_tag(1)=" << type_tag(1)
              << " type_tag(true)=" << type_tag(true) << "\n";

    // P4/P5/P6: STL container + algorithms + lambda
    std::vector<int> v = {5, 2, 8, 1, 9, 3};
    std::sort(v.begin(), v.end());
    int sum = std::accumulate(v.begin(), v.end(), 0);
    auto evens = std::count_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; });
    std::cout << "P6  sorted front=" << v.front() << " back=" << v.back()
              << " sum=" << sum << " evens=" << evens << "\n";

    // P4: map
    std::map<std::string, int> ages = {{"alice", 30}, {"bob", 25}};
    ages["carol"] = 40;
    std::cout << "P4  map[bob]=" << ages["bob"] << " size=" << ages.size() << "\n";

    // P7/P8: copy vs move
    std::cout << "P8  push_back(lvalue) -> ";
    std::vector<Tracked> tv;
    tv.reserve(4);                 // avoid reallocation noise
    Tracked t1;
    tv.push_back(t1);              // copy
    std::cout << "P8  push_back(rvalue) -> ";
    tv.push_back(Tracked());       // move
    tv.emplace_back();             // constructed in place (no move/copy line)
    std::cout << "P8  emplace_back      -> (in place: no copy/move)\n";

    // P9: perfect forwarding (rvalue -> move)
    std::cout << "P9  forward_into(rvalue) -> ";
    Tracked f = forward_into(Tracked());
    (void)f;

    // P11
    std::cout << "P11 ";
    print_all("mixed", 42, 3.14, 'x');

    // P13: vector capacity growth
    std::vector<int> g;
    std::size_t caps = 0; std::size_t last = 0;
    for (int i = 0; i < 20; ++i) { g.push_back(i); if (g.capacity()!=last){last=g.capacity();++caps;} }
    std::cout << "P13 vector grew capacity " << caps << " times for 20 pushes\n";
    return 0;
}
