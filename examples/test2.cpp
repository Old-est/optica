#include "../include/optica/impl/properties2.hpp"
#include <print>

int main (int argc, char *argv[]) {
    optica::ArityProperty<optica::Exact<2>> a; 
    static_assert(optica::HasArityProperty<int, double, decltype(a)>, "Sos");
    std::println("N Args = {}", a.GetNArgs());
    return 0;
}
