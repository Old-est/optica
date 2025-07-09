#include <iostream>
#include <optica/optica.hpp>

int main(int argc, char *argv[]) {
  constexpr auto parser = optica::Parser(
      optica::Option<"Day", int>() | optica::DefaultValue(3) |
          optica::ShortName("A") | optica::Arity<optica::Exact<1>>(),
      optica::Option<"Week", int>() | optica::Arity<optica::One>());
  auto result = parser.Parse("-A 2 --Week 666");

  auto day = result.Get<"Day">();
  if (day) {
    std::println("Day is {}", day.value());
  }

  auto week = result.Get<"Week">();
  if (week) {
    std::println("Week is {}", week.value());
  }
}
