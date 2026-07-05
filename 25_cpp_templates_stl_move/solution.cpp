/*
 * Topic 25: C++ Templates, STL & Move Semantics
 * Compile: g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o solution && ./solution
 *
 * Each problem is illustrated below; main() demonstrates all of them.
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
T my_max(const T& a, const T& b) { return (a > b) ? a : b; }

/* P2: class template - a tiny generic stack. */
template <typename T>
class Stack {
public:
    void push(const T& v) { data_.push_back(v); }
    T pop() { T v = data_.back(); data_.pop_back(); return v; }
    bool empty() const { return data_.empty(); }
    std::size_t size() const { return data_.size(); }
private:
    std::vector<T> data_;
};

/* P3: template specialization - customize for bool. */
template <typename T>
const char* type_tag(const T&) { return "generic"; }
template <>
const char* type_tag<bool>(const bool&) { return "bool-special"; }

/* P7/P8: a type that reports copy vs move so we can SEE move semantics. */
class Tracked {
public:
    Tracked() : id_(++counter_) {}
    Tracked(const Tracked&)            { std::cout << "      [copy]\n"; }
    Tracked(Tracked&&) noexcept        { std::cout << "      [move]\n"; }
    Tracked& operator=(const Tracked&) { std::cout << "      [copy=]\n"; return *this; }
    Tracked& operator=(Tracked&&) noexcept { std::cout << "      [move=]\n"; return *this; }
private:
    int id_;
    static int counter_;
};
int Tracked::counter_ = 0;

/* P11: variadic template - type-safe print. */
template <typename... Args>
void print_all(const Args&... args) {
    ((std::cout << args << ' '), ...);   // C++17 fold expression
    std::cout << '\n';
}

/* P9: perfect forwarding. */
template <typename T>
Tracked forward_into(T&& x) {
    return Tracked(std::forward<T>(x));  // preserves lvalue/rvalue-ness
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
