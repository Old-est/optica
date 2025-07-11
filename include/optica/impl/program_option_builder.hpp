#pragma once

#include "properties.hpp"

namespace optica {

template <Property... T> struct ProgramOption;

template <Property... Props> struct ProgramOptionBuilder : Props... {
  using Properties = std::tuple<Props...>;
  constexpr ProgramOptionBuilder() noexcept = default;
  template <typename... Ts>
  constexpr ProgramOptionBuilder(Ts &&...args) noexcept
      : Props(std::forward<Ts>(args))... {};

  constexpr operator ProgramOption<Props...>() && {
    return ProgramOption<Props...>{std::move(static_cast<Props &>(*this))...};
  }
};

template <typename... T, typename... U>
constexpr auto operator|(ProgramOptionBuilder<T...> &&left,
                         ProgramOptionBuilder<U...> &&right) {
  return ProgramOptionBuilder<T..., U...>{
      std::move(static_cast<T &>(left))...,
      std::move(static_cast<U &>(right))...};
}

template <FixedString Name> constexpr auto Flag() noexcept {
  return ProgramOptionBuilder<ArgumentProperty<Name, bool>>{};
}

template <FixedString Name, typename ValueType>
constexpr auto Option() noexcept {
  return ProgramOptionBuilder<ArgumentProperty<Name, ValueType>>{};
}

template <Arity arity> constexpr auto NArgs() noexcept {
  return ProgramOptionBuilder<ArityProperty<arity>>{};
}

template <typename ValueType>
constexpr auto DefaultValue(ValueType &&value) noexcept {
  using Decayed = std::decay_t<ValueType>;
  return ProgramOptionBuilder<DefaultValueProperty<Decayed>>{
      DefaultValueProperty<Decayed>{std::forward<ValueType>(value)}};
}

constexpr auto ShortName(std::string_view short_name) noexcept {
  return ProgramOptionBuilder<ShortNameProperty>{short_name};
}

constexpr auto Required() noexcept {
  return ProgramOptionBuilder<RequiredProperty>{};
}

} // namespace optica
