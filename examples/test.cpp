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
      optica::Positional<"year", int>(), optica::Positional<"month", int>(),
      optica::Positional<"day", int>(),
      optica::Opt<"mem", int>() | optica::ShortName<"m">());

  constexpr std::string_view cmd = "2025 -m 33 07 22";
  auto res = parser.Parse(cmd);

  std::println("Mem: {}", res.Get<"mem">().value());
  std::println("Year: {}", res.Get<"year">().value());
  std::println("Month: {}", res.Get<"month">().value());
  std::println("Day: {}", res.Get<"day">().value());

  return 0;
}
