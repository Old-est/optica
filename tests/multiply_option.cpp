#include <catch2/catch_all.hpp>
#include <optica/optica.hpp>

constexpr auto parser = optica::Parser(
    optica::Opt<"week", std::array<std::string, 7>>() |
    optica::ShortName<"w">() | optica::Arity<optica::Exact<7>>());

TEST_CASE("Option can consume multiply objects by long name", "[option]") {
  constexpr std::string_view cmd = "--week Mon,Tue,Wed,Thu,Fri,Sat,Sun";
  auto result = parser.Parse(cmd);
  auto val = result.Get<"week">().value();
  std::array<std::string, 7> days = {"Mon", "Tue", "Wed", "Thu",
                                     "Fri", "Sat", "Sun"};
  REQUIRE_THAT(val, Catch::Matchers::RangeEquals(days));
}

TEST_CASE("Option can consume multiply objects by long name with =",
          "[option]") {
  constexpr std::string_view cmd = "--week=Mon,Tue,Wed,Thu,Fri,Sat,Sun";
  auto result = parser.Parse(cmd);
  auto val = result.Get<"week">().value();
  std::array<std::string, 7> days = {"Mon", "Tue", "Wed", "Thu",
                                     "Fri", "Sat", "Sun"};
  REQUIRE_THAT(val, Catch::Matchers::RangeEquals(days));
}

TEST_CASE("Option can consume multiply objects by short name", "[option]") {
  constexpr std::string_view cmd = "-w Mon,Tue,Wed,Thu,Fri,Sat,Sun";
  auto result = parser.Parse(cmd);
  auto val = result.Get<"week">().value();
  std::array<std::string, 7> days = {"Mon", "Tue", "Wed", "Thu",
                                     "Fri", "Sat", "Sun"};
  REQUIRE_THAT(val, Catch::Matchers::RangeEquals(days));
}
