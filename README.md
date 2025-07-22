# optica

It's small library
optica stands from

# Basic usage


```cpp
#include <optica/optica.hpp>

int main(int argc, char* argv[]) {
    constexpr auto parser = optica::Parser(optica::Option<"Day", int>() |
                                           optica::NArgs<optica::One>());
    auto result = parser.Parse(argc, argv);
    auto day = result.Get<"Day">();
    if (day) {
        std::println("Today is {} day", day);
    }
    return 0;
}
```
