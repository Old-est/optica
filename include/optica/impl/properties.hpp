#pragma once

#include <concepts>

#include "fixed_string.hpp"

namespace optica {

/**
 * @struct BaseProperty
 * @brief Represents family of properties
 *
 * Used just for internal checks that some type
 * is property
 *
 * @tparam Derived Property type
 *
 * @remark Used in CRTP style for not having N BaseProperty instances
 */
template <typename Derived>
struct BaseProperty {};

/**
 * @concept Property
 * @brief Checks if a type is property type
 */
template <typename T>
concept Property = std::derived_from<T, BaseProperty<T>>;

/**
 * @brief Type converter for extracting Tags
 */
template <Property Prop>
using PropertyTag_t = Prop::Tag;

/**
 * @brief Calculates number types holding speciefic PropertyTag
 *
 * @tparam Tag Selected Tag
 * @return std::size_t number types with Tag
 */
template <typename Tag, Property... Props>
consteval std::size_t CountTags() {
  return (0 + ... + std::is_same_v<Tag, PropertyTag_t<Props>>);
}

/**
 * @class NamePropertyTag
 * @brief Tag for NameProperty
 *
 */
struct NamePropertyTag {};

/**
 * @struct NameProperty
 * @brief Property holds name set by template parameter
 *
 * @tparam NameValue Compile time string literal specifies name
 */
template <FixedString NameValue>
struct NameProperty : BaseProperty<NameProperty<NameValue>> {
  using Tag = NamePropertyTag;
  /**
   * @brief Function that gives Name for the property in compiletime
   *
   * @return FixedString value. Name which is hold by NameProperty
   */
  constexpr static auto GetName() noexcept { return NameValue; }
};

namespace details {
template <typename T>
struct is_name_property : std::false_type {};

template <FixedString Name>
struct is_name_property<NameProperty<Name>> : std::true_type {};
}  // namespace details

/**
 * @concept NamePropertyType
 * @brief Checks if type is NameProperty
 */
template <typename T>
concept NamePropertyType = details::is_name_property<T>::value;

/**
 * @concept HasNamePropertyType
 * @brief Checks if parameters pack contains NameProperty
 */
template <typename... Ts>
concept HasNamePropertyType = (NamePropertyType<Ts> || ...);

/**
 * @class ValuePropertyTag
 * @brief Tag for ValueProperty
 *
 */
struct ValuePropertyTag {};

/**
 * @struct ValueProperty
 * @brief Proprty holds ValueType
 *
 * @tparam ValueType which type property holds
 */
template <typename ValueType>
struct ValueProperty : BaseProperty<ValueProperty<ValueType>> {
  using Tag = ValuePropertyTag;
  using value_type = ValueType;

  /**
   * @brief Function that returns ValueType
   *
   * @return ValueType
   * @warning This function must be used only inside decltype operator
   */
  constexpr ValueType GetValueType() const noexcept;
};

namespace details {
template <typename T>
struct is_value_property : std::false_type {};

template <typename T>
struct is_value_property<ValueProperty<T>> : std::true_type {};
}  // namespace details

/**
 * @concept ValuePropertyType
 * @brief Checks if type is ValueProperty
 */
template <typename T>
concept ValuePropertyType = details::is_value_property<T>::value;

/**
 * @concept HasValuePropertyType
 * @brief Checks if parameters pack contains ValueProperty
 */
template <typename... Ts>
concept HasValuePropertyType = (ValuePropertyType<Ts> || ...);

/**
 * @class RequiredPropertyTag
 * @brief Tag for RequiredProperty
 *
 */
struct RequiredPropertyTag {};

/**
 * @struct RequiredProperty
 * @brief Property tels is required value or not
 */
struct RequiredProperty : BaseProperty<RequiredProperty> {
  using Tag = RequiredPropertyTag;
};

/**
 * @concept RequiredPropertyType
 * @brief Checks if type is RequiredProperty
 */
template <typename T>
concept RequiredPropertyType = std::is_same_v<T, RequiredProperty>;

/**
 * @concept HasRequiredPeopertyType
 * @brief Checks if parameters pack contains RequiredProperty
 */
template <typename... Ts>
concept HasRequiredPeopertyType = (RequiredPropertyType<Ts> || ...);

/**
 * @class DefaultValueTag
 * @brief Tag for DefaultValueProperty
 *
 */
struct DefaultValuePropertyTag {};

/**
 * @struct DefaultValueProperty
 * @brief Represents DefaultValueProperty
 *
 * @tparam ValueType type of the default_value
 */
template <typename ValueType>
struct DefaultValueProperty : BaseProperty<DefaultValueProperty<ValueType>> {
  using Tag = DefaultValuePropertyTag;
  using default_value_type = ValueType;
  default_value_type default_value;

  /**
   * @brief Constructs Default value Property
   *
   * @param default_value Default value that will be stored
   */
  constexpr explicit DefaultValueProperty(ValueType default_value) noexcept
      : default_value(default_value) {}

  /**
   * @brief Returns default value
   *
   * @return default_value_type default value stored in property
   */
  constexpr const default_value_type &GetDefaultValue() const noexcept {
    return default_value;
  }
};

namespace details {
template <typename T>
struct is_default_value_type : std::false_type {};

template <typename T>
struct is_default_value_type<DefaultValueProperty<T>> : std::true_type {};
}  // namespace details

/**
 * @concept DefaultValuePropertyType
 * @brief Checks if T is DefaultValueProperty
 */
template <typename T>
concept DefaultValuePropertyType = details::is_default_value_type<T>::value;

/**
 * @concept HasDefaultValuePropertyType
 * @brief Checks if parameters pack contains DefaultValueProperty
 */
template <typename... Ts>
concept HasDefaultValuePropertyType = (DefaultValuePropertyType<Ts> || ...);

/**
 * @class ShortNameTag
 * @brief Tag for ShortNameProperty
 *
 */
struct ShortNamePropertyTag {};

/**
 * @struct ShortNameProperty
 * @brief Represents short name property for option
 */
template <FixedString ShortName>
struct ShortNameProperty : BaseProperty<ShortNameProperty<ShortName>> {
  using Tag = ShortNamePropertyTag;
  char short_name;

  /**
   * @brief Returns string_view on short name
   *
   * @return string_view stored short_name
   */
  constexpr static auto GetShortName() noexcept { return ShortName; }
};

namespace details {
template <typename T>
struct is_short_name_type : std::false_type {};

template <FixedString Value>
struct is_short_name_type<ShortNameProperty<Value>> : std::true_type {};
}  // namespace details

/**
 * @concept ShortNamePropertyType
 * @brief Checks if T is ShortNameProperty
 */
template <typename T>
concept ShortNamePropertyType = details::is_short_name_type<T>::value;

/**
 * @concept HasShortNamePropertyType
 * @brief Checks if parameters pack contains ShortNameProperty
 */
template <typename... Ts>
concept HasShortNamePropertyType = (ShortNamePropertyType<Ts> || ...);

/**
 * @class BindPropertyTag
 * @brief Tag for BindProperty
 *
 */
struct BindPropertyTag {};

/**
 * @struct BindProperty
 * @brief Represents BindProperty
 *
 * Tells library that this option \ref Option holds reference
 * to value where store parsed value
 *
 * @tparam ValueType Type of bound value
 */
template <typename ValueType>
struct BindProperty : BaseProperty<BindProperty<ValueType>> {
  using Tag = BindPropertyTag;

  /**
   * @brief Constructs BindProperty
   *
   * @param val Value that will be captured by property
   */
  constexpr BindProperty(ValueType &val) noexcept : value(val) {}

  ValueType &value;
};

namespace details {
template <typename T>
struct is_bind_property : std::false_type {};

template <typename T>
struct is_bind_property<BindProperty<T>> : std::true_type {};
}  // namespace details

/**
 * @concept BindPropertyType
 * @brief Checks if T is BindProperty
 */
template <typename T>
concept BindPropertyType = details::is_bind_property<T>::value;

/**
 * @concept HasBindPropertyType
 * @brief Checks if parameters pack contains BindProperty
 */
template <typename... Ts>
concept HasBindPropertyType = (BindPropertyType<Ts> || ...);

template <typename... Ts>
concept SameTypes =
    (std::is_same_v<Ts,
                    typename std::tuple_element<0, std::tuple<Ts...>>::type> &&
     ...);

struct VarianPropertyTag {};

/**
 * @struct VariantProperty
 * @brief Represents VariantProperty
 *
 * @tparam ValueType Type of holding variants
 * @tparam N Size of variants
 */
template <typename ValueType, std::size_t N>
struct VariantProperty : BaseProperty<VariantProperty<ValueType, N>> {
  using Tag = VarianPropertyTag;

  /**
   * @brief Constructs VariantProperty
   *
   * @tparam Args Type of params must be same
   * @param args Variant values
   */
  template <typename... Args>
    requires(sizeof...(Args) > 1)
  constexpr VariantProperty(Args &&...args) noexcept
      : variants{{std::forward<Args>(args)...}} {}

  constexpr const auto &GetVariants() const noexcept { return variants; }

  std::array<ValueType, N> variants;
};

template <typename... Args>
VariantProperty(Args &&...args)
    -> VariantProperty<std::common_type_t<Args...>, sizeof...(Args)>;

namespace details {
template <typename T>
struct is_variant_property : std::false_type {};

template <typename T, std::size_t N>
struct is_variant_property<VariantProperty<T, N>> : std::true_type {};
}  // namespace details

/**
 * @concept VariantPropertyType
 * @brief Checks if type T is VariantProperty
 */
template <typename T>
concept VariantPropertyType = details::is_variant_property<T>::value;

/**
 * @concept HasVariantPropertyType
 * @brief Checks if parameters pack contains VariantProperty
 */
template <typename... Ts>
concept HasVariantPropertyType = (VariantPropertyType<Ts> || ...);

template <std::size_t N>
struct Exact {
  static constexpr std::size_t GetNumberArgs() noexcept { return N; }
};

namespace details {
template <typename T>
struct is_exact_arity : std::false_type {};

template <std::size_t N>
struct is_exact_arity<Exact<N>> : std::true_type {};
}  // namespace details

template <typename T>
concept ExactArity = details::is_exact_arity<T>::value;

using One = Exact<1>;
using Two = Exact<2>;
using Three = Exact<3>;

struct ArityPropertyTag {};

template <typename T>
struct ArityProperty : BaseProperty<ArityProperty<T>> {
  using Tag = ArityPropertyTag;
  using arity_type = T;

  constexpr static arity_type GetArityType() noexcept;
};

namespace details {
template <typename T>
struct is_arity_property : std::false_type {};

template <typename T>
struct is_arity_property<ArityProperty<T>> : std::true_type {};
}  // namespace details

template <typename T>
concept ArityPropertyType = details::is_arity_property<T>::value;

template <typename... Ts>
concept HasArityPropertyType = (ArityPropertyType<Ts> || ...);

}  // namespace optica
