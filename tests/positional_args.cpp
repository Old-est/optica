#include <catch2/catch_all.hpp>
#include <optica/optica.hpp>

TEST_CASE("You can parse positional arg", "[positional]") {
  constexpr auto parser = optica::Parser(optica::Positional<"day", int>());
  constexpr std::string_view cmd = "31";
  auto result = parser.Parse(cmd);
  auto value = result.Get<"day">().value();
  REQUIRE(value == 31);
}

TEST_CASE("You can parse multiply positional arg", "[positional]") {
  constexpr auto parser = optica::Parser(optica::Positional<"year", int>(),
                                         optica::Positional<"month", int>(),
                                         optica::Positional<"day", int>());

  constexpr std::string_view cmd = "2025 07 22";
  auto res = parser.Parse(cmd);
  auto year = res.Get<"year">().value();
  auto month = res.Get<"month">().value();
  auto day = res.Get<"day">().value();

  REQUIRE(year == 2025);
  REQUIRE(month == 7);
  REQUIRE(day == 22);
}

TEST_CASE("You can mix positional and general args", "[positional]") {
  constexpr auto parser = optica::Parser(
      optica::Positional<"year", int>(), optica::Positional<"month", int>(),
      optica::Positional<"day", int>(), optica::Opt<"guest", std::string>());

  constexpr std::string_view cmd = "--guest Dmitrii 2025 07 22";
  auto res = parser.Parse(cmd);
  auto year = res.Get<"year">().value();
  auto month = res.Get<"month">().value();
  auto day = res.Get<"day">().value();
  auto guest = res.Get<"guest">().value();

  REQUIRE(year == 2025);
  REQUIRE(month == 7);
  REQUIRE(day == 22);
  REQUIRE(guest == "Dmitrii");
}

struct Student {
  int id{};
  std::string name;
  std::string surname;
};

template <>
struct optica::TypeParser<Student> {
  static Student ParseValue(const optica::Token& token) {
    auto res = token.ExtractTokenUnits<3>();
    return {.id = optica::TypeParser<int>::ParseValue(res[0]),
            .name = optica::TypeParser<std::string>::ParseValue(res[1]),
            .surname = optica::TypeParser<std::string>::ParseValue(res[2])};
  }
};

TEST_CASE("Positional argument can hold custom type", "[positional]") {
  constexpr auto parser =
      optica::Parser(optica::Positional<"student", Student>());
  constexpr std::string_view cmd = "{42, Dmitrii, Tupitsyn}";

  auto result = parser.Parse(cmd);
  auto val = result.Get<"student">().value();

  REQUIRE(val.id == 42);
  REQUIRE(val.name == "Dmitrii");
  REQUIRE(val.surname == "Tupitsyn");
}
