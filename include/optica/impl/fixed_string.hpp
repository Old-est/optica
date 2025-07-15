#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace optica {

/**
 * @class FixedString
 * @brief Represents compile time string
 *
 * @tparam N (length of the string)
 *
 * @note Used as NTTP for naming options
 */
template <std::size_t N> struct FixedString {

  /**
   * @brief Constructs fixed size string from string literal
   *
   * @param str Actual string literal
   * @tparam N Fixed size string length minus terminal \0
   */
  constexpr FixedString(const char (&str)[N + 1]) noexcept {
    std::copy_n(str, N, data.data());
  }

  /**
   * @brief Constructs fixed size string from array of chars
   *
   * @param str Array of chars
   * @tparam N fixed size string length
   */
  constexpr FixedString(const std::array<char, N> str) noexcept : data(str) {}

  /**
   * @brief Converts fixed string into string_view
   */
  constexpr operator std::string_view() const noexcept {
    return {data.data(), data.size()};
  }

  /**
   * @brief Converts Fixed string into general string
   */
  constexpr operator std::string() const noexcept {
    return {data.data(), data.size()};
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
        if (data[i] != other.data[i]) {
          return false;
        }
      }
      return true;
    }
  }

  std::array<char, N> data;
};

/**
 * @brief Deduction hint
 *
 * It allows to deduce the right type of the FixedString
 * without trailing \0 code in c++ string literals
 *
 * @param str Actual string literal
 * @tparam N size of string literal with trailing \0
 */
template <std::size_t N>
FixedString(const char (&str)[N]) -> FixedString<N - 1>;
} // namespace optica
