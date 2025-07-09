#pragma once

#include <charconv>
#include <optional>
#include <string_view>
namespace optica {

template <typename T>
constexpr std::optional<T> ParseType(std::string_view data) {
  static_assert(sizeof(T) == 0, "No try_parse specialization for this type T");
  return std::nullopt;
}

template <> constexpr std::optional<int> ParseType<int>(std::string_view data) {
  int val{};
  auto [ptr, ec] = std::from_chars(data.data(), data.data() + data.size(), val);
  if (std::errc() == ec) {
    return val;
  }
  return std::nullopt;
}

} // namespace optica
