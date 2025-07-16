#pragma once
#include "option_builder.hpp"
#include <print>
#include <string>

namespace optica {

/**
 * @struct Option
 * @brief Represents Combined sets of \ref Property
 *
 */
template <Property... Properties> struct Option : Properties... {
  /**
   * @brief Constructs Option class from Properties
   *
   * @tparam Props Properties types
   * @param props Properties
   */
  template <typename... Props>
  constexpr Option(Props &&...props) noexcept
      : Properties(std::forward<Props>(props))... {}

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

} // namespace optica
