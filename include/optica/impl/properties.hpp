#pragma once

#include "fixed_string.hpp"
#include <string>

namespace optica {
template <FixedString Name, typename ValueType> struct ArgumentProperty {
  using value_type = ValueType;
  static constexpr FixedString name = Name;
};

struct ShortNameProperty {
  constexpr explicit ShortNameProperty(std::string_view name) noexcept
      : short_name(name) {}
  std::string_view short_name;
};

struct RequiredProperty {
  bool required;
};

template <typename ValueType> struct DefaultValueProperty {
  constexpr explicit DefaultValueProperty(ValueType value) noexcept
      : default_value(value) {}

  ValueType default_value;
};

template <std::size_t N> struct Exact {
  static constexpr std::size_t size = N;

  static constexpr std::size_t GetSize() noexcept { return N; }
};

using One = Exact<1>;

template <typename T> struct ArityProperty : T {};

template <typename> struct is_arity_property : std::false_type {};

template <typename U>
struct is_arity_property<ArityProperty<U>> : std::true_type {};

template <typename T>
constexpr bool is_arity_property_v = is_arity_property<T>::value;

template <typename... T>
concept HasArity = (is_arity_property_v<T> || ...);

template <typename T, typename U>
concept HasDefaultValue = std::is_base_of_v<DefaultValueProperty<U>, T>;

template <typename T>
concept HasRequired = std::is_base_of_v<RequiredProperty, T>;

template <typename T>
concept HasShortName = std::is_base_of_v<ShortNameProperty, T>;

template <typename T> struct is_argument_property : std::false_type {};

template <FixedString Name, typename ValueType>
struct is_argument_property<ArgumentProperty<Name, ValueType>>
    : std::true_type {};

template <typename T>
constexpr bool is_argument_property_v = is_argument_property<T>::value;

template <typename... Ts>
concept HasArgument = (is_argument_property_v<Ts> || ...);
} // namespace optica
