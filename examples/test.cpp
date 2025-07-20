#include <optica/optica.hpp>
#include <print>

static double val = 5.0;

int main(int argc, char *argv[]) {
  constexpr auto parser =
      optica::Parser(optica::Opt<"Sosal", int>() | optica::Required() |
                         optica::ShortName<"s">(),
                     optica::Opt<"Ebal", std::string>());

  auto kek = parser.Parse("--Sosal 25 --Ebal=sosal");

  std::println("Value is: {}", kek.Get<"Sosal">().value());
  std::println("Value is: {}", kek.Get<"Ebal">().value());

  return 0;
}
