#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
namespace optica {

/**
 * @brief Compiletime friendly fixed size string
 *        used as NTTP
 *
 * @tparam N String size + 1
 */
template <std::size_t N> struct FixedString {

  /**
   * @brief Constructs fixed size string
   *
   * @param str Actual string as string literal
   * @tparam N String Size
   */
  constexpr FixedString(const char (&str)[N]) noexcept {
    std::copy_n(str, N, value);
  }

  /**
   * @brief Checks if 2 strings are equal
   *
   * @param other Other string may be with different size
   * @return bool
   */
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

  /**
   * @brief Converts fixed size string to string_view
   *
   * @return std::string_view
   */
  constexpr operator std::string_view() const noexcept { return value; }

  /**
   * @brief storage for symbols
   */
  char value[N]{};
};
} // namespace optica
