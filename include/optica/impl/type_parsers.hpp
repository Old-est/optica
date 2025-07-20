#pragma once

#include <charconv>

#include "token.hpp"
namespace optica {
template <typename Type>
struct TypeParser;

template <>
struct TypeParser<int> {
  static int ParseValue(const Token& token) {
    int val{};
    auto [ptr, ec] = std::from_chars(token.GetTokenData().data(),
                                     token.GetTokenData().end(), val);
    return val;
  }
};

template <>
struct TypeParser<double> {
  static double ParseValue(const Token& token) {
    double val{};
    auto [ptr, ec] = std::from_chars(token.GetTokenData().data(),
                                     token.GetTokenData().end(), val);
    return val;
  }
};

template <>
struct TypeParser<std::string> {
  static std::string ParseValue(const Token& token) {
    return std::string(token.GetTokenData());
  }
};
}  // namespace optica
