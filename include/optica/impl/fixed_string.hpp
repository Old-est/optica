#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
namespace optica {

template <std::size_t N> struct FixedString {

  constexpr FixedString(const char (&str)[N]) noexcept {
    std::copy_n(str, N, value);
  }

  template <std::size_t U>
  constexpr bool operator==(const FixedString<U> &other) const noexcept {
    if constexpr (N != U) {
      return false;
    } else {
      for (std::size_t i = 0; i < N; ++i) {
        if (value[i] != other.value[i]) {
          return false;
        }
      }
      return true;
    }
  }

  constexpr operator std::string_view() const noexcept { return value; }

  char value[N]{};
};
} // namespace optica
