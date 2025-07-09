#pragma once
#include "fixed_string.hpp"
#include "properties.hpp"
#include "type_parsers.hpp"

#include <iostream>
#include <optional>
#include <print>
#include <utility>
#include <vector>
namespace optica {

template <typename... Options> class Parser;

template <typename... Options> struct ProgramOption;

template <typename... T> struct ProgramOptionBuilder : T... {
  using base_types = std::tuple<T...>;

private:
  constexpr ProgramOptionBuilder() noexcept = default;

public:
  template <typename... U>
  constexpr ProgramOptionBuilder(U &&...options)
      : T(std::forward<U>(options))... {}
};

template <FixedString Name> constexpr auto Flag() noexcept {
  return ProgramOptionBuilder<ArgumentProperty<Name, bool>>{
      ArgumentProperty<Name, bool>{}};
}

template <FixedString Name, typename ValueType>
constexpr auto Option() noexcept {
  return ProgramOptionBuilder<ArgumentProperty<Name, ValueType>>{
      ArgumentProperty<Name, ValueType>{}};
}

constexpr auto Required() noexcept {
  return ProgramOptionBuilder<RequiredProperty>{RequiredProperty{}};
}

template <typename T> constexpr auto Arity() noexcept {
  return ProgramOptionBuilder<ArityProperty<T>>{ArityProperty<T>{}};
}

constexpr auto ShortName(std::string_view short_name) noexcept {
  return ProgramOptionBuilder<ShortNameProperty>{short_name};
}

template <typename ValueType>
constexpr auto DefaultValue(ValueType value) noexcept {
  return ProgramOptionBuilder<DefaultValueProperty<ValueType>>{
      std::move(value)};
}

template <typename... T, typename... U>
constexpr auto operator|(ProgramOptionBuilder<T...> &&left,
                         ProgramOptionBuilder<U...> &&right) {
  return ProgramOptionBuilder<T..., U...>{
      std::move(static_cast<T &>(left))...,
      std::move(static_cast<U &>(right))...};
}

template <typename... T> struct ProgramOption : T... {

  using base_types = std::tuple<T...>;

  constexpr ProgramOption(const ProgramOptionBuilder<T...> &builder) noexcept
    requires HasArgument<T...>
      : T(static_cast<const T &>(builder))... {}
  constexpr ProgramOption(ProgramOptionBuilder<T...> &&builder) noexcept
    requires HasArgument<T...>
      : T(static_cast<T &&>(builder))... {}

private:
  // Чтобы только Parser мог устанавливать значения
  template <typename... Options> friend class Parser;

  template <typename RetValue, typename Iter>
  bool TryConsume(Iter &start, Iter end,
                            std::optional<RetValue> &out) const {
    using Option = ProgramOption<T...>;

    if (start == end) {
      return false;
    }

    std::string_view token = *start;
    std::string_view name_candidate;

    // 1. Пытаемся по длинному имени
    if (token.starts_with("--")) {
      name_candidate = token.substr(2);
      if (name_candidate == this->name.value) {
        // std::println("Matched long: {}", name_candidate);
        ++start;
        // std::cout << std::boolalpha << (start == end) << '\n';
        if (!ConsumeArgs(start, end, out)) {
          std::cerr << "Cant Parse Option: " << *(start - 1) << '\n';
          std::exit(2);
        }
        return true;
        // return ConsumeArgs(start, end, out);
      }
    }

    // 2. Пытаемся по короткому имени
    if constexpr (HasShortName<Option>) {
      if (token.starts_with("-")) {
        name_candidate = token.substr(1);
        if (name_candidate == this->short_name) {
          // std::println("{}", "Matched short: {}", name_candidate);
          ++start;
          if (!ConsumeArgs(start, end, out)) {
            std::cerr << "Cant Parse Option: " << *(start - 1) << '\n';
            std::exit(2);
          }
          return true;
          // return ConsumeArgs(start, end, out);
        }
      }
    }

    // 3. Если опция обязательная, но не найдена
    if constexpr (HasRequired<Option>) {
      // std::println("Option '{}' is required but not found",
      // this->name.value);
    }

    return false;
  }

  template <typename RetValue, typename Iter>
  bool ConsumeArgs(Iter &start, Iter end, std::optional<RetValue> &out) const {
    if constexpr (HasArity<T...>) {
      size_t count = this->GetSize(); // Например, Exact<1> → 1
      size_t total_size = 0;
      for (auto it = start; it != start + count; ++it) {
        total_size += it->size();
      }

      std::string result;
      result.reserve(total_size);

      bool first = true;
      for (auto it = start; it != start + count; ++it) {
        if (!first) {
          result.push_back(' ');
        }
        result.append(*it);
        first = false;
      }

      out = ParseType<RetValue>(result);
      start += count;
      // out = ...;
      return true;
    }
    return false;
  }

  template <typename RetValue, typename Iter>
  constexpr bool TryParse(std::vector<std::string_view> &data, Iter &scan_start,
                          RetValue &out) const {

    std::string_view name;
    if (scan_start->starts_with("--")) {
      name = scan_start->substr(2);
    } else {
      if constexpr (HasShortName<ProgramOption<T...>>) {
        if (scan_start->starts_with('-')) {
          name = scan_start->substr(1);
        }
      }
    }
    std::println("{}", name);

    return true;
    // auto pars_result = ParseType<RetValue>(data);
    // if constexpr (HasDefaultValue<ProgramOption<T...>, RetValue>) {
    //   return this->default_value;
    // }
    //
    // if constexpr (HasRequired<ProgramOption<T...>>) {
    //   if (pars_result) {
    //     return pars_result;
    //   } else {
    //     std::println("Option {} mark required but arg isnt provided",
    //                  this->name);
    //     std::exit(3);
    //   }
    // }
    // return std::nullopt;

    // auto scan_start = res.begin();
    //   auto try_find_and_parse = [&res, &scan_start](auto &tuple_elemet) {
    //     using OptionType = decltype(tuple_elemet);
    //     // NOTE: Еще одна функция
    //     std::string_view name;
    //     if (scan_start->starts_with("--")) {
    //       name = scan_start->substr(2);
    //     } else {
    //       if constexpr (HasShortName<OptionType>) {
    //         if (scan_start->starts_with('-')) {
    //           name = scan_start->substr(1);
    //         }
    //       } else {
    //         std::cout << "Sosal\n";
    //       }
    //     }
    //
    //     if constexpr (HasArity<OptionType>) {
    //       std::cout << "Sosal2\n";
    //       std::size_t size = ArityType::GetSize();
    //       std::cout << size << '\n';
    //     }
    //     std::cout << name << '\n';
    //   };
    //
    //   std::apply(try_find_and_parse, types_);
    //
    //   for (auto s : res) {
    //     std::cout << "[" << s << "]\n";
    //   }
    //   return result;
  }
};

template <typename... Opts>
ProgramOption(Opts &&...) -> ProgramOption<std::decay_t<Opts>...>;

template <typename T> struct ToProgramOption {
  using type = T;
};

template <typename... Ts> struct ToProgramOption<ProgramOptionBuilder<Ts...>> {
  using type = ProgramOption<Ts...>;
};

template <typename T>
using ToProgramOptionT = typename ToProgramOption<std::decay_t<T>>::type;

} // namespace optica
