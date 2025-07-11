#pragma once

#include <ranges>
#include <string_view>
#include <vector>

namespace optica::utils {

inline std::vector<std::string_view> SplitString(std::string_view data,
                                                 char splitter) {
  auto split_string = data | std::ranges::views::split(splitter);
  std::vector<std::string_view> results;
  for (const auto &item : split_string) {
    results.emplace_back(item.begin(), item.end());
  }
  return results;
};

} // namespace optica::utils
