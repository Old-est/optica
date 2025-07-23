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

### Optional arguments

Optional arguments can be created with `optica::Opt` function. You need specify it's name and value type this option is going to hold.
Additionaly you can combine each option with certain properties:

1. `optica::Required()` -- sets `RequiredProperty` on your option. This means parser will check if user specified this option in CMD;
2. `optica::DefaultValue()` -- sets `DefaultValueProperty`. If this property exists in option default value will be placed in result if option isn't observed in CMD;
3. `optica::ShortName()` -- sets `ShortNameProperty`. With this property option can be specified with it's short name with prefix `-`;
4. `optica::Variant()` -- specifies possible variants for option;
5. `optica::Bind()` -- is't possible to bind variable to speciefic option. Parsed value will be stored in that variable;
6. `optica::Arity()` -- specifies `ArityProperty`. This property tells library how many tokens must be consumed

## Roadmap

- [x] Positional arguments support
- [x] Option support
- [x] Parsing custom types
- [x] Parsing sequence of options into vector
- [ ] Subcommands support
- [ ] Parsing from config file
- [ ] Excluding groups
