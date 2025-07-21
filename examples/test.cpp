#include <optica/optica.hpp>
#include <print>

static double val = 5.0;

struct Mom {
  int a;
  double b;
  std::string c;
};

template <>
struct optica::TypeParser<Mom> {
  static Mom ParseValue(const optica::Token& token) {
    auto res = token.ExtractTokenUnits<3>();
    return {.a = optica::TypeParser<int>::ParseValue(res[0]),
            .b = optica::TypeParser<double>::ParseValue(res[1]),
            .c = optica::TypeParser<std::string>::ParseValue(res[2])};
  }
};

int main(int argc, char* argv[]) {
  constexpr auto parser = optica::Parser(
      optica::Opt<"Sosal", std::array<int, 3>>() | optica::Required() |
          optica::ShortName<"s">() | optica::Arity<optica::Three>(),
      optica::Opt<"Ebal", Mom>());

  auto kek = parser.Parse("--Sosal 25 3 7 --Ebal={1,, mom}");

  std::println("Value is: {}", kek.Get<"Sosal">().value());
  auto compound_val = kek.Get<"Ebal">().value();
  std::println("Value is: {} {} {}", compound_val.a, compound_val.b,
               compound_val.c);

  return 0;
}
