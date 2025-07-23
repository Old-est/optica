# optica

## Library features

- Header only
- Module support

## Quick Start

Any parser starts with `optica::Parser` constructor

```cpp
constexpr my_parser = optica::Parser(optica::Opt<"name", std::string>);
```

In general you should specify all args in one constructor, but you can create some intermediates.

### Simple example

As a simple example you can see code below:

```cpp
#include <optica/optica.hpp>

int main(int argc, char *argv[]) {
    constexpr auto parser = optica::Parser(optica::Opt<"compression_level", int> |
                                           optica::ShortName<"l"> |
                                           optica::Variant(0, 1, 2),
                                           optica::Positional<"src", std::string>);

    int level{};
    std::string file_name;
    try {
        auto parsing_result = parser.Parse(argc, argv);
        level = parsing_result.Get<"compression_level">().value();
        file_name = parsing_result.Get<"src">().value();
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
```

## Roadmap

- [x] Positional arguments support
- [x] Option support
- [x] Parsing custom types
- [x] Parsing sequence of options into vector
- [-] Subcommands support
- [-] Parsing from config file
- [-] Excluding groups
