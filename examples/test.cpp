#include <optica/optica.hpp>
#include <print>

static double val = 5.0;

int main(int argc, char *argv[]) {
  constexpr auto mem = optica::Opt<"Sosal", int>() | optica::Required() |
                       optica::DefaultValue(2) | optica::ShortName('s') |
                       optica::Bind(val) | optica::Variant(1, 2, 3, 4, 5, 6, 7);

  constexpr auto real_opt = optica::CreateOption(mem);

  std::println("{}", real_opt.GenerateDescription());

  return 0;
}
