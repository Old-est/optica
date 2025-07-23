#pragma once
#include <print>
#include <string>

#include "option_builder.hpp"
#include "token.hpp"
#include "type_parsers.hpp"

namespace optica {
enum class ResultType {
  Ok,
  False,
  PositionalMatch,
};
template <typename ValueType>
struct ConsumeResult {
  ResultType type;
  std::size_t advance;
  ValueType value;
};

/**
 * @struct Option
 * @brief Represents Combined sets of \ref Property
 *
 */
template <Property... Properties>
struct Option : Properties... {
  /**
   * @brief Constructs Option class from Properties
   *
   * @tparam Props Properties types
   * @param props Properties
   */
  constexpr Option(Properties &&...props) noexcept
      : Properties(std::forward<Properties>(props))... {}

  /**
   * @brief Generates Description for option
   *
   * @return Formatted option text
   */
  [[nodiscard]] constexpr std::string GenerateDescription() const noexcept {
    std::string result;
    if constexpr (HasNamePropertyType<Properties...>) {
      std::format_to(std::back_inserter(result), "Name: --{}\n",
                     std::string_view(this->GetName()));
    }
    if constexpr (HasShortNamePropertyType<Properties...>) {
      std::format_to(std::back_inserter(result), "  Short Name: -{}\n",
                     this->GetShortName());
    }

    if constexpr (HasRequiredPeopertyType<Properties...>) {
      std::format_to(std::back_inserter(result), "  Required: {}\n", true);
    } else {
      std::format_to(std::back_inserter(result), "  Required: {}\n", false);
    }

    if constexpr (HasDefaultValuePropertyType<Properties...>) {
      std::format_to(std::back_inserter(result), "  Default value: {}\n",
                     this->GetDefaultValue());
    }

    if constexpr (HasVariantPropertyType<Properties...>) {
      std::format_to(std::back_inserter(result), "  Possible variants: {}",
                     this->GetVariants());
    }

    return result;
  }

  auto TryConsume(TokenIterator start, TokenIterator end) const {
    using ParsedValue = decltype(this->GetValueType());
    using ReturnType = ConsumeResult<ParsedValue>;

    auto token_type = (*start).GetTokenType();

    if (token_type == Token::TokenType::Word or
        token_type == Token::TokenType::CompoundName) {
      if constexpr (HasPositionalPropertyType<Properties...>) {
        auto res = Consume(start, end);
        res.advance = res.advance - 1;
        res.type = ResultType::PositionalMatch;
        return res;
      } else {
        return ReturnType{.type = ResultType::False, .advance = 0};
      }
    }
    auto token_name = (*start).GetTokenData();

    if (token_type == Token::TokenType::LongName) {
      if constexpr (HasNamePropertyType<Properties...>) {
        if (token_name == this->GetName()) {
          return Consume(++start, end);
        }
      } else {
        return ReturnType{.type = ResultType::False, .advance = 0};
      }
    }

    if (token_type == Token::TokenType::ShortName) {
      if constexpr (HasShortNamePropertyType<Properties...>) {
        if (token_name == this->GetShortName()) {
          return Consume(++start, end);
        }
      } else {
        return ReturnType{.type = ResultType::False, .advance = 0};
      }
    }

    return ReturnType{.type = ResultType::False, .advance = 0};
  }

  auto Consume(TokenIterator start, TokenIterator end) const {
    using ParsedValue = decltype(this->GetValueType());
    using ReturnType = ConsumeResult<ParsedValue>;

    if constexpr (!HasArityPropertyType<Properties...> and
                  !std::is_same_v<ParsedValue, bool>) {
      constexpr int tokens_number = 1;
      auto value = TypeParser<ParsedValue>::ParseValue(*(start));
      return ReturnType{.type = ResultType::Ok, .advance = 2, .value = value};
    }
    if constexpr (HasArityPropertyType<Properties...>) {
      using ArityType = decltype(this->GetArityType());
      if constexpr (ExactArity<ArityType>) {
        constexpr std::size_t size = ArityType::GetNumberArgs();
        ParsedValue res;
        for (std::size_t i = 0; i < size; ++i) {
          res[i] = TypeParser<ParsedValue>::ParseValue(*(start));
          ++start;
        }
        return ReturnType{
            .type = ResultType::Ok, .advance = size + 1, .value = res};
      }
    };
  }
};

/**
 * @brief Deduction hint
 *
 * @tparam Props Property types
 */
template <typename... Props>
Option(Props &&...) -> Option<std::decay_t<Props>...>;

/**
 * @brief Manual Cast to Option from \ref OptionBuilder
 *
 * @tparam Props Property types
 * @param value OptionBuilder value
 * @return Option
 */
template <typename... Props>
constexpr auto CreateOption(OptionBuilder<Props...> &&value) noexcept {
  return Option<Props...>{std::move(static_cast<Props &>(value))...};
}

/**
 * @brief Manual Cast to Option from \ref OptionBuilder
 *
 * @tparam Props Property types
 * @param value OptionBuilder value
 * @return Option
 */
template <typename... Props>
constexpr auto CreateOption(OptionBuilder<Props...> value) noexcept {
  return Option<Props...>{(static_cast<Props &>(value))...};
}

namespace details {
template <typename T>
struct is_option : std::false_type {};

template <typename... Ts>
struct is_option<Option<Ts...>> : std::true_type {};
}  // namespace details

/**
 * @concept OptionType
 *
 * @brief Checks if T is Option
 */
template <typename T>
concept OptionType = details::is_option<T>::value;

}  // namespace optica
