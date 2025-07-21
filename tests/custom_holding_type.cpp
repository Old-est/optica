#include <catch2/catch_all.hpp>
#include <optica/optica.hpp>

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

constexpr auto parser = optica::Parser(optica::Opt<"student", Student>() |
                                       optica::ShortName<"s">());

TEST_CASE("Option can parse custom types by long name", "[option]") {
  constexpr std::string_view cmd = "--student {42, Dmitrii, Tupitsyn}";
  auto result = parser.Parse(cmd);
  auto val = result.Get<"student">().value();

  REQUIRE(val.id == 42);
  REQUIRE(val.name == "Dmitrii");
  REQUIRE(val.surname == "Tupitsyn");
}

TEST_CASE("Option can parse custom types by long name with =", "[option]") {
  constexpr std::string_view cmd = "--student={42, Dmitrii, Tupitsyn}";
  auto result = parser.Parse(cmd);
  auto val = result.Get<"student">().value();

  REQUIRE(val.id == 42);
  REQUIRE(val.name == "Dmitrii");
  REQUIRE(val.surname == "Tupitsyn");
}

TEST_CASE("Option can parse custom types by short name", "[option]") {
  constexpr std::string_view cmd = "-s {42, Dmitrii, Tupitsyn}";
  auto result = parser.Parse(cmd);
  auto val = result.Get<"student">().value();

  REQUIRE(val.id == 42);
  REQUIRE(val.name == "Dmitrii");
  REQUIRE(val.surname == "Tupitsyn");
}
