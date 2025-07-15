#pragma once

#include <tuple>

namespace optica {
template <typename... Opts> class Parser {
  using Options = std::tuple<Opts...>;
};
} // namespace optica
