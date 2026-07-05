/*
 * Topic 23: C++ Virtual Functions & Polymorphism
 * Compile: g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o solution && ./solution
 *
 * Each problem is illustrated by a small class hierarchy; main() demonstrates all.
 */
#include <iostream>
#include <memory>
#include <vector>
#include <string>

/* P1/P2/P3: abstract base with a pure virtual and a VIRTUAL DESTRUCTOR. */
class Shape {
public:
    virtual double area() const = 0;        // P2: pure virtual -> abstract
    virtual std::string name() const = 0;
    virtual ~Shape() { }                     // P3: virtual dtor (essential!)
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159265 * r_ * r_; }  // P4: override
    std::string name() const override { return "Circle"; }
private:
    double r_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double area() const override { return w_ * h_; }
    std::string name() const override { return "Rectangle"; }
private:
    double w_, h_;
};

/* P5: static vs dynamic binding. */
class Base {
public:
    void non_virtual() const { std::cout << "Base::non_virtual "; } // static bind
    virtual void virt() const { std::cout << "Base::virt "; }       // dynamic bind
    virtual ~Base() = default;
};
class Derived : public Base {
public:
    void non_virtual() const { std::cout << "Derived::non_virtual "; } // hides
    void virt() const override { std::cout << "Derived::virt "; }
};

int main() {
    // P1: runtime dispatch through base pointers held in a container.
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(2.0));
    shapes.push_back(std::make_unique<Rectangle>(3.0, 4.0));

    std::cout << "P1  polymorphic areas:\n";
    for (const auto& s : shapes)
        std::cout << "      " << s->name() << " area=" << s->area() << "\n";
    // P3: deleting via unique_ptr<Shape> calls the right dtor because it's virtual.

    // P5: static vs dynamic binding via a Base pointer to a Derived.
    Derived d;
    Base* bp = &d;
    std::cout << "P5  ";
    bp->non_virtual();   // Base::   (resolved by static type)
    bp->virt();          // Derived:: (resolved by dynamic type)
    std::cout << "\n";

    // P9: dynamic_cast for a safe downcast.
    Shape* sp = shapes[0].get();
    if (auto* c = dynamic_cast<Circle*>(sp))
        std::cout << "P9  dynamic_cast to Circle OK, area=" << c->area() << "\n";
    if (dynamic_cast<Rectangle*>(sp) == nullptr)
        std::cout << "P9  dynamic_cast Circle->Rectangle = nullptr (as expected)\n";

    std::cout << "P3  note: without a virtual ~Shape(), deleting via Shape* is UB/leak\n";
    std::cout << "P12 note: 'Shape s = *sp;' would SLICE off the derived part\n";
    return 0;
}
