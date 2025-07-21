#include <catch2/catch_all.hpp>
#include <optica/optica.hpp>

constexpr auto parser =
    optica::Parser(optica::Opt<"day", int>() | optica::ShortName<"d">());

TEST_CASE("Option can be parsed by long name", "[option]") {
  constexpr std::string_view cmd = "--day 3";
  auto result = parser.Parse(cmd);
  REQUIRE(result.Get<"day">().value() == 3);
}
TEST_CASE("Option can be parsed by long name with =", "[option]") {
  constexpr std::string_view cmd = "--day=7";
  auto result = parser.Parse(cmd);
  REQUIRE(result.Get<"day">().value() == 7);
}
TEST_CASE("Option can be parsed by short name", "[option]") {
  constexpr std::string_view cmd = "-d 42";
  auto result = parser.Parse(cmd);
  REQUIRE(result.Get<"day">().value() == 42);
}
