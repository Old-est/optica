#pragma once

#include <charconv>
#include <string_view>
namespace optica {

/**
 * @brief Parsing function for std::string_view
 *
 * @tparam T Type into string should be parsed
 * @param data Actual std::string_view with data
 * @return bool that shows is parsing successful
 */
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
