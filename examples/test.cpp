#include <iostream>
#include <optica/optica.hpp>
#include <print>

int main(int argc, char *argv[]) {

  constexpr auto parser = optica::Parser(
      optica::Option<"Day", int>() | optica::NArgs<optica::One>() |
      optica::DefaultValue(13) | optica::ShortName("D"));
  auto result = parser.Parse("-D 2");
  auto day = result.Get<"Day">();
  if (day) {
    std::println("Day is {}", day.value());
  }
}
