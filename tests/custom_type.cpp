#include <catch2/catch_test_macros.hpp>
#include <optica/optica.hpp>

struct SimpleVec {
  int x{};
  int y{};
  int z{};
};

template <>
bool optica::ParseType<SimpleVec>(std::string_view data, SimpleVec &res) {
  auto splitted = optica::utils::SplitString(data, ' ');
  std::from_chars(splitted[0].data(), splitted[0].end(), res.x);
  std::from_chars(splitted[1].data(), splitted[1].end(), res.y);
  std::from_chars(splitted[2].data(), splitted[2].end(), res.z);
  return true;
}

static constexpr auto Parser = optica::Parser(
    optica::Option<"Center", SimpleVec>() | optica::NArgs<optica::One>());

TEST_CASE("Parsing custom types") {
  auto result = Parser.Parse("--Center 1 2 3");
  auto value = result.Get<"Center">();
  REQUIRE(value.value().x == 1);
  REQUIRE(value.value().y == 2);
  REQUIRE(value.value().z == 3);
}
