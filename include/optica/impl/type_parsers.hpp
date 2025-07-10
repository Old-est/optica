#pragma once

#include <charconv>
#include <optional>
#include <string_view>
namespace optica {

template <typename T> constexpr bool ParseType(std::string_view data, T &) {
  static_assert(sizeof(T) == 0, "No try_parse specialization for this type T");
  return false;
}

template <> constexpr bool ParseType<int>(std::string_view data, int &res) {
  int val{};
  auto [ptr, ec] = std::from_chars(data.data(), data.data() + data.size(), val);
  if (std::errc() == ec) {
    res = val;
    return true;
  }
  return false;
}

} // namespace optica
