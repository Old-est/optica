#pragma once

#include "fixed_string.hpp"
#include <string_view>
#include <type_traits>
namespace optica {

/**
 * @class PropertyBase
 * @brief Base class for any kind CMD option
 *
 */
template <typename Derived> struct PropertyBase {};

/**
 * @concept Property
 * @brief Concept for determining is T Property or not
 *
 * @tparam T
 */
template <typename T>
concept Property = std::is_base_of_v<PropertyBase<T>, T>;

/**
 * @class OneOrMore
 * @brief Represents option takes at least one argument
 *
 */
struct OneOrMore {};

/**
 * @class Exact
 * @brief Represents option takes exact number
 *
 * @tparam Size
 */
template <std::size_t Size> struct Exact {

  /**
   * @brief Returns number of args
   *
   * @return std::size_t
   */
  static constexpr std::size_t GetNumberArgs() noexcept { return Size; }
};

using One = Exact<1>;
using Two = Exact<2>;
using Three = Exact<3>;
using Four = Exact<4>;

namespace details {
template <typename T> struct is_exact : std::false_type {};
template <std::size_t N> struct is_exact<Exact<N>> : std::true_type {};
} // namespace details

/**
 * @brief Concept determining is Arity numeric
 *
 * @tparam T
 */
template <typename T>
concept IsNumericArity = details::is_exact<T>::value;

/**
 * @brief Concept determining is Arity one of not numeric
 *
 * @tparam T
 */
template <typename T>
concept IsTypedArity = std::is_same_v<T, OneOrMore>;

/**
 * @brief Concept for any related to Arguments type
 *
 * @tparam T
 */
template <typename T>
concept Arity = IsNumericArity<T> || IsTypedArity<T>;

/**
 * @class ArityProperty
 *
 * @brief Represents Arity property (Possible number of arguments)
 *
 * @tparam Value requires concept Arity
 */
template <Arity Value>
struct ArityProperty : PropertyBase<ArityProperty<Value>> {

  /**
   * @brief Returns number of arguments if Value is Numeric Arity
   *
   * @return std::size_t
   */
  static constexpr std::size_t GetNumberArgs() noexcept
    requires IsNumericArity<Value>
  {
    return Value::GetNumberArgs();
  }
};

namespace details {
template <typename T> struct is_arity : std::false_type {};
template <typename T> struct is_arity<ArityProperty<T>> : std::true_type {};
} // namespace details

/**
 * @concept IsArityProperty
 * @brief IsArityOrNot
 *
 * @tparam T
 */
template <typename T>
concept IsArityProperty = details::is_arity<T>::value;

template <typename... T>
concept HasArityProperty = ((IsArityProperty<T>) || ...);

struct RequiredProperty : PropertyBase<RequiredProperty> {};

template <typename T>
concept IsRequiredProperty = std::is_same_v<T, RequiredProperty>;

template <typename... T>
concept HasRequiredProperty = ((IsRequiredProperty<T>) || ...);

template <typename ValueType>
struct DefaultValueProperty : PropertyBase<DefaultValueProperty<ValueType>> {
  ValueType default_value;

  constexpr DefaultValueProperty() noexcept = default;
  constexpr explicit DefaultValueProperty(ValueType value) noexcept
      : default_value(value) {}

  constexpr ValueType GetDefaultValue() const noexcept { return default_value; }
};

namespace details {
template <typename T> struct is_default_value : std::false_type {};

template <typename T>
struct is_default_value<DefaultValueProperty<T>> : std::true_type {};
} // namespace details

template <typename T>
concept IsDefaultValueProperty = details::is_default_value<T>::value;

template <typename... T>
concept HasDefaultValueProperty = ((IsDefaultValueProperty<T>) || ...);

template <FixedString Name, typename ValueType>
struct ArgumentProperty : PropertyBase<ArgumentProperty<Name, ValueType>> {
  using value_type = ValueType;
  static constexpr std::string_view GetName() noexcept { return Name; }
  static constexpr ValueType GetValueType() noexcept;
};

namespace details {
template <typename T> struct is_argument : std::false_type {};

template <FixedString Name, typename ValueType>
struct is_argument<ArgumentProperty<Name, ValueType>> : std::true_type {};
} // namespace details

template <typename T>
concept IsArgumentProperty = details::is_argument<T>::value;

template <typename... T>
concept HasArgumentProperty = ((IsArgumentProperty<T>) || ...);

struct ShortNameProperty : PropertyBase<ShortNameProperty> {
  std::string_view short_name;

  constexpr ShortNameProperty() noexcept = default;
  constexpr ShortNameProperty(std::string_view short_name) noexcept
      : short_name(short_name) {}
};

template <typename T>
concept IsShortNameProperty = std::is_same_v<ShortNameProperty, T>;

template <typename... T>
concept HasShortNameProperty = ((IsShortNameProperty<T>) || ...);

} // namespace optica
