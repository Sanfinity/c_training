/*
 * Topic 24: C++ Smart Pointers & RAII -- PRACTICE TEMPLATE
 * Fill in each function/method body marked TODO. Answers are in solution.cpp.
 * Compile: g++ -std=c++17 -Wall -O2 practice.cpp -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <iostream>
#include <memory>
#include <string>

/* P1/P7: a minimal RAII wrapper. Acquire in ctor, release in dtor.
 * The message order at run time proves deterministic cleanup. */
class ScopedResource {
public:
    explicit ScopedResource(std::string name) : name_(std::move(name)) {
        /* TODO: implement. HINT: this is the ACQUIRE step; announce that name_ was acquired. */
    }
    ~ScopedResource() {
        /* TODO: implement. HINT: this is the RELEASE step; announce that name_ is being released. */
    }
private:
    std::string name_;
};

/* P9: custom deleter type for a C resource (simulated FILE*). */
struct FakeFile { const char* path; };
struct FileCloser {
    void operator()(FakeFile* f) const {
        /* TODO: implement. HINT: this custom deleter should announce the close, then delete f. */
    }
};

/* P4/P8: weak_ptr breaks a shared_ptr cycle.
 * Node.next is shared, Node.prev is weak -> no leak. */
struct Node {
    int value;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node>   prev;   // weak: does NOT keep prev alive
    explicit Node(int v) : value(v) {}
    ~Node() { std::cout << "      [~Node " << value << "]\n"; }
};

/* P11: unique_ptr member -> Rule of Zero (no manual dtor needed). */
class Widget {
public:
    Widget() : buf_(std::make_unique<int[]>(4)) {
        /* TODO: implement. HINT: fill the 4 slots (e.g. i*i); the unique_ptr member handles cleanup for you. */
    }
    int at(int i) const { return buf_[i]; }
private:
    std::unique_ptr<int[]> buf_;
};

int main() {
    // P2: unique_ptr, exclusive ownership
    auto up = std::make_unique<int>(42);
    std::cout << "P2  unique_ptr value     = " << *up << "\n";

    // P10: transfer ownership with move
    std::unique_ptr<int> up2 = std::move(up);
    std::cout << "P10 after move: up=" << (up ? "set" : "null")
              << " up2=" << *up2 << "\n";

    // P3/P13: shared_ptr and use_count
    auto sp1 = std::make_shared<std::string>("shared");
    {
        auto sp2 = sp1;                       // refcount -> 2
        std::cout << "P3  shared use_count     = " << sp1.use_count() << "\n";
    }                                         // sp2 gone -> refcount 1
    std::cout << "P13 use_count after scope= " << sp1.use_count() << "\n";

    // P1/P7: RAII scope - watch acquire/release order (LIFO)
    std::cout << "P1  RAII scope:\n";
    {
        ScopedResource r1("A");
        ScopedResource r2("B");
    }   // B released before A (reverse construction order)

    // P9: custom deleter
    std::cout << "P9  custom deleter:\n";
    {
        std::unique_ptr<FakeFile, FileCloser> f(new FakeFile{"config.txt"});
        std::cout << "      using " << f->path << "\n";
    }   // FileCloser runs here

    // P4/P8: cycle avoided via weak_ptr
    std::cout << "P8  weak_ptr breaks cycle:\n";
    {
        auto a = std::make_shared<Node>(1);
        auto b = std::make_shared<Node>(2);
        a->next = b;      // shared
        b->prev = a;      // weak -> no cycle, both destroyed
    }   // ~Node 1 and ~Node 2 both print -> no leak

    // P11: Rule of Zero
    Widget w;
    std::cout << "P11 Widget buf[3]        = " << w.at(3) << "\n";
    return 0;
}
