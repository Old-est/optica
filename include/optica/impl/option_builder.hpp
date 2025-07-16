#pragma once
#include "properties.hpp"
#include <utility>

namespace optica {

template <Property... Props> struct Option;

/**
 * @struct OptionBuilder
 * @brief Struct that provide abillity to store and accumulate different
 * properties
 * @tparam Props Different properties
 */
template <Property... Props> struct OptionBuilder : Props... {
  /**
   * @brief Default constructor
   */
  constexpr OptionBuilder() noexcept = default;
  /**
   * @brief Constructs OptionBuilder from properties
   *
   * @tparam Ts Types satisfying \ref Property concept and \ref UniqueProperties
   * @param args Properties
   */
  template <typename... Ts>
  constexpr OptionBuilder(Ts &&...args) noexcept
      : Props(std::forward<Ts>(args))... {}

  /**
   * @brief Converting operator from OptionBuilder to Option
   *
   * @return Option<Props...> Built option
   */
  constexpr operator Option<Props...>() && noexcept {
    return Option<Props...>{std::move(static_cast<Props &>(*this))...};
  }
};

/**
 * @concept UniqueProperties
 * @brief Checks if all properties are unique
 *
 * @note In general cpp disallow inheritance from type more
 * than one time. So simple cases are just fine. This concept
 * orineted on cases such as:
 *
 * @code{.cpp}
 * // This is invalid even if typical c++ checks says they're different types
 * auto option = optica::Opt<"Day", int> | optica::Opt<"Week", double>;
 * @endcode
 *
 */
template <typename... Properties>
concept UniqueProperties = ([]() constexpr {
  return ((CountTags<PropertyTag_t<Properties>, Properties...>() <= 1) && ...);
})();

/**
 * @concept MatchingDefaultAndValueTypes
 * @brief Checks if Holding ValueType and DefaultValueType are same
 *
 */
template <typename... Properties>
concept MatchingDefaultAndValueTypes = std::is_same_v<
    decltype(std::declval<OptionBuilder<Properties...>>().GetValueType()),
    std::decay_t<decltype(std::declval<OptionBuilder<Properties...>>()
                              .GetDefaultValue())>>;

/**
 * @concept HasMatchingDefaultValueType
 * @brief Checks if set of Properties has DefaultValueProperty and are it same
 * as Holding ValueType
 *
 *
 * @code{.cpp}
 * // This is valid because holding int and DefaultValue accept same type
 * auto option = optica::Opt<"Day", int> | optica::DefaultValue(3);
 *
 * // This is invalid because DefaultValue is string literal
 * auto option = optica::Opt<"Day", int> | optica::DefaultValue("Monday");
 * @endcode
 *
 */
template <typename... Properties>
concept HasMatchingDefaultValueType =
    (!HasDefaultValuePropertyType<Properties...>) ||
    (MatchingDefaultAndValueTypes<Properties...>);

/**
 * @concept MatchingVariantAndValueTypes
 * @brief Checks if Holding ValueType and VariantProperty type are same
 *
 */
template <typename... Properties>
concept MatchingVariantAndValueTypes = std::is_same_v<
    decltype(std::declval<OptionBuilder<Properties...>>().GetValueType()),
    typename std::decay_t<decltype(std::declval<OptionBuilder<Properties...>>()
                                       .GetVariants())>::value_type>;

/**
 * @concept HasMatchingVariantPropertyType
 * @brief Checks if set of the Properties has VariantProperty and if it has
 * checks equality holding ValueType and VariantProperty Type
 *
 * @code{.cpp}
 * // This is valid because holding value is type of int and variant types are
 * int too auto option = optica::Opt<"Day", int> | optica::Variant(1, 2, 3, 4,
 * 5, 6, 7);
 *
 * // This is invalid because variant type are string literal
 * auto option = optica::Opt<"Day", int> | optica::Variant("Mon", "Tue", "Wed",
 * "Thu", "Fri", "Sat", "Sun");
 * @endcode
 */
template <typename... Properties>
concept HasMatchingVariantPropertyType =
    (!HasVariantPropertyType<Properties...>) ||
    (MatchingVariantAndValueTypes<Properties...>);

/**
 * @concept ValidOrderExpression
 * @brief Checks if Option expression starts with Opt
 */
template <typename... Properties>
concept ValidOrderExpression =
    HasNamePropertyType<Properties...> && HasValuePropertyType<Properties...>;

/**
 * @concept ValidPropertyExpression
 * @brief Cchecks if Property expression is valid
 */
template <typename... Properties>
concept ValidPropertyExpression = UniqueProperties<Properties...> &&
                                  HasMatchingDefaultValueType<Properties...> &&
                                  HasMatchingVariantPropertyType<Properties...>;

/**
 * @brief Pipe operator for accumulating properties
 *
 * @param left OptionBuilder<Ts...> Left operand must contain name and value
 * @param right OptionBuilder<Us...> Right operand
 * @return OptionBuilder<Ts...,Us...> New option with properties from left and
 * right operands
 * @tparam Ts Properties inside lhf OptionBuilder
 * @tparam Us Properties inside rhf OptionBuilder
 *
 * @remark Left operand requires \ref ValidOrderExpression
 * and whole expression requires \ref ValidPropertyExpression
 * for strict ordering in construction option
 *
 * @code{.cpp}
 * // Option1 is valid because Opt func sets Name and ValueType
 * auto Option1 = optica::Opt<"Day", int>() | optica::Required();
 * // Option2 is invalid because starts from Required
 * auto Option2 = optica::Required() | optica::Opt<"Day", int>;
 * @endcode
 */
template <typename... Ts, typename... Us>
  requires ValidOrderExpression<Ts...> && ValidPropertyExpression<Ts..., Us...>
constexpr auto operator|(OptionBuilder<Ts...> left,
                         OptionBuilder<Us...> right) noexcept {
  return OptionBuilder<Ts..., Us...>{std::move(static_cast<Ts &>(left))...,
                                     std::move(static_cast<Us &>(right))...};
}

/**
 * @brief Creates Option with Name and ValueType
 *
 * @tparam Name FixedString CompileTime string
 * @tparam ValueType value which holds option
 */
template <FixedString Name, typename ValueType> constexpr auto Opt() noexcept {
  return OptionBuilder<NameProperty<Name>, ValueProperty<ValueType>>{};
}

/**
 * @brief Sets Required Flag
 *
 * If not provided in chain of pipe operators means no required
 */
constexpr auto Required() noexcept { return OptionBuilder<RequiredProperty>{}; }

/**
 * @brief Creates Flag option
 *
 * @tparam Name FixedString compiletime name
 */
template <FixedString Name> constexpr auto Flag() noexcept {
  return OptionBuilder<NameProperty<Name>, ValueProperty<bool>>{};
}

/**
 * @brief Sets Default value for option
 *
 * @tparam ValueType type of the default value
 * @param value default value itself
 */
template <typename ValueType>
constexpr auto DefaultValue(ValueType value) noexcept {
  return OptionBuilder<DefaultValueProperty<ValueType>>{
      DefaultValueProperty<ValueType>{value}};
}

/**
 * @brief Sets ShortName for option
 *
 * @param short_name short name for option
 */
constexpr auto ShortName(char short_name) noexcept {
  return OptionBuilder<ShortNameProperty>{ShortNameProperty{short_name}};
}

/**
 * @brief Sets BindProperty for option
 *
 * @tparam ValueType Type of holding bind
 * @param value Value binded
 *
 * @note
 * You cant create constexpr \ref Option if your value isn't constexpr
 * available. In this case create non-constexpr option or don't use
 * \ref BindProperty
 *
 * @code{.cpp}
 * static double speed = 5.0;
 *
 * constexpr auto option = optica::Option<"speed", double> |
 * optica::Bind(speed);
 *
 * // In this case it's okey because taking ref or pointer to static variable
 * // is possible in constexpr context
 *
 * double height{};
 *
 * auto option_height = optica::Option<"height", double> | optica::Bind(height);
 *
 * // In case of non static variable you should choose non-constexpr version
 * @endcode
 */
template <typename ValueType> constexpr auto Bind(ValueType &value) noexcept {
  return OptionBuilder<BindProperty<ValueType>>{BindProperty<ValueType>{value}};
}

/**
 * @brief Sets VariantProperty for option
 *
 * @tparam ValueType type of variants
 * @param vals variants
 */
template <typename... ValueType>
requires SameTypes<ValueType...>
constexpr auto Variant(ValueType &&...vals) noexcept {
  using ReturnType = std::common_type_t<ValueType...>;
  return OptionBuilder<VariantProperty<ReturnType, sizeof...(ValueType)>>{
      VariantProperty<ReturnType, sizeof...(ValueType)>{
          std::forward<ValueType>(vals)...}};
}

} // namespace optica
