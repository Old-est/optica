#pragma once

#include <tuple>

#include "option.hpp"
#include "token.hpp"

namespace optica {

constexpr void ParseByLongName(TokenIterator &it) {
  // ... разбираем аргумент
  ++it;  // обязательно
}

namespace details {

template <OptionType... Opts>
consteval auto ConstructNamesArray() noexcept {
  std::array<std::string, sizeof...(Opts)> res = {
      std::string(Opts::GetName())...};
  return res;
}

template <typename T>
constexpr std::string GetShortNameOrEmpty() {
  if constexpr (requires { T::GetShortName(); }) {
    return T::GetShortName();
  } else {
    return "";
  }
}

template <OptionType... Opts>
constexpr auto ConstructShortNamesArray() {
  std::array<std::string, sizeof...(Opts)> all = {
      std::string(GetShortNameOrEmpty<Opts>())...};

  return all;
}

template <typename Tuple>
struct tuple_types;

template <typename... Ts>
struct tuple_types<std::tuple<Ts...>> {
  template <typename F>
  static constexpr auto expand(F &&f) {
    return std::forward<F>(f).template operator()<Ts...>();
  }
};

template <typename T>
constexpr auto make_program_option(T &&t) {
  using Decayed = std::decay_t<T>;

  if constexpr (OptionType<Decayed>) {
    return std::forward<T>(t);
  } else {
    return tuple_types<typename Decayed::Properties>::expand(
        [&]<typename... Props>() {
          return static_cast<Option<Props...>>(std::forward<T>(t));
        });
  }
}

template <FixedString Name, int idx>
struct EnsureIndexExists {
  static_assert(idx != -1, "ProgramOption with the given name not found.");
  static constexpr std::size_t value = idx;
};

}  // namespace details

template <OptionType... Ts>
class Parser;

template <OptionType... Options>
class ParseResult {
  using ValueType = std::tuple<
      std::optional<decltype(std::declval<Options>().GetValueType())>...>;

 public:
  constexpr ParseResult() = default;

  template <FixedString Name>
  constexpr auto Get() const noexcept {
    constexpr int idx_raw =
        GetIndexByName<Name>(std::index_sequence_for<Options...>{});
    constexpr int idx = details::EnsureIndexExists<Name, idx_raw>::value;
    return std::get<idx>(values_);
  }

 private:
  friend Parser<Options...>;

  template <std::size_t I, typename T>
  constexpr void SetValue(T &&value) {
    if (!std::get<I>(values_).has_value()) {
      std::get<I>(values_) = std::forward<T>(value);
      return;
    }
    throw std::invalid_argument("Duplicate option def");
  }

  template <FixedString Name, std::size_t... Is>
  static consteval int GetIndexByName(std::index_sequence<Is...> idxs) {
    auto names = details::ConstructNamesArray<Options...>();
    for (std::size_t i = 0; i < names.size(); ++i) {
      if (names[i] == static_cast<std::string_view>(Name)) {
        return i;
      }
    };
    return -1;
  }

 private:
  ValueType values_;
};

template <OptionType... Options>
class Parser {
 public:
  using OptionsValue = std::tuple<Options...>;
  using ParseResultType = ParseResult<Options...>;

  template <typename... Args>
  constexpr Parser(Args &&...opts) noexcept
      : options_(details::make_program_option(std::forward<Args>(opts))...) {}

  ParseResultType Parse(std::string_view data) const {
    ParseResultType result{};
    auto tokenizer = Tokenizer{data};
    auto begin = tokenizer.begin();
    auto end = tokenizer.end();

    constexpr auto size_of_params = sizeof...(Options);
    std::size_t parsed{};

    for (; begin != end;) {
      int steps_count{0};

      auto traverse_options = [&]<std::size_t... Is>(
                                  std::index_sequence<Is...>) {
        bool was_matched =
            (([&] {
               auto consume_result =
                   std::get<Is>(options_).TryConsume(begin, end);
               if (consume_result.type == ResultType::PositionalMatch) {
                 if (std::get<Is>(result.values_).has_value()) {
                   return false;
                 }
                 std::get<Is>(result.values_) = std::move(consume_result.value);
                 steps_count = consume_result.advance;
                 ++parsed;
                 return true;
               }
               if (consume_result.type == ResultType::Ok) {
                 if (std::get<Is>(result.values_).has_value()) {
                   std::string message;
                   std::format_to(
                       std::back_inserter(message),
                       "ERROR: You're trying set option {} more than 1 time",
                       *begin);
                   throw std::invalid_argument(message);
                 }
                 std::get<Is>(result.values_) = std::move(consume_result.value);
                 steps_count = consume_result.advance;
                 ++parsed;
                 return true;
               }
               return false;
             }()) ||
             ...);

        if (!was_matched) {
          std::string message;
          std::format_to(std::back_inserter(message),
                         "ERROR: Unknown Argument: {}", *begin);
          throw std::invalid_argument(message);
        }
      };
      traverse_options(std::make_index_sequence<size_of_params>{});
      std::advance(begin, steps_count);

      if (parsed >= size_of_params) {
        break;
      }
      // NOTE: Check for required stuff
    }
    return result;
  }

 private:
  OptionsValue options_;
};

template <typename... Args>
Parser(Args &&...args) -> Parser<
    decltype(details::make_program_option(std::forward<Args>(args)))...>;

}  // namespace optica
