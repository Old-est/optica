#pragma once

#include "program_option.hpp"
#include "utils.hpp"
#include <iostream>
namespace optica {

namespace details {

template <typename Tuple> struct tuple_types;

template <typename... Ts> struct tuple_types<std::tuple<Ts...>> {
  template <typename F> static constexpr auto expand(F &&f) {
    return std::forward<F>(f).template operator()<Ts...>();
  }
};

template <typename T> constexpr auto make_program_option(T &&t) {
  using Decayed = std::decay_t<T>;

  if constexpr (IsProgramOption<Decayed>) {
    return std::forward<T>(t);
  } else {
    return tuple_types<typename Decayed::Properties>::expand(
        [&]<typename... Props>() {
          return static_cast<ProgramOption<Props...>>(std::forward<T>(t));
        });
  }
}

template <FixedString Name, int idx> struct EnsureIndexExists {
  static_assert(idx != -1, "ProgramOption with the given name not found.");
  static constexpr std::size_t value = idx;
};

} // namespace details

template <typename... Values> class ParserResult;

template <typename... Opts> class Parser {
public:
  template <typename... Args>
  constexpr Parser(Args &&...opts) noexcept
      : options_(details::make_program_option(std::forward<Args>(opts))...) {}

  ParserResult<Opts...> Parse(std::string_view data) const {
    using ReturnType = ParserResult<Opts...>;
    auto result = ReturnType{};

    auto lexems = utils::SplitString(data, ' ');

    auto start = lexems.begin();
    auto end = lexems.end();
    for (;;) {

      constexpr auto Size = sizeof...(Opts);

      [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        bool matched = (([&] {
                          auto [status, value] =
                              std::get<Is>(options_).TryConsume(start, end);
                          if (status != ParsingStatus::Error) {
                            std::get<Is>(result.values_) =
                                std::move(value); // или GetName() вместо Is
                          }
                          return status == ParsingStatus::Ok;
                        }()) ||
                        ...);

        if (!matched and (start != end)) {
          std::cerr << "Unknown Argument: " << *start << '\n';
          std::exit(1);
        }
      }(std::make_index_sequence<Size>{});

      if (start == end) {
        break;
      }
    }

    return result;
  }

private:
  std::tuple<Opts...> options_;
};

template <typename... Args>
Parser(Args &&...args) -> Parser<
    decltype(details::make_program_option(std::forward<Args>(args)))...>;

template <typename... Opts> class ParserResult {
public:
  constexpr ParserResult() noexcept = default;

  template <FixedString Name> constexpr auto Get() const noexcept {
    constexpr int idx_raw =
        GetIndexByName<Name>(std::index_sequence_for<Opts...>{});
    constexpr int idx = details::EnsureIndexExists<Name, idx_raw>::value;
    return std::get<idx>(values_);
  }

private:
  template <FixedString Name, std::size_t... Is>
  static consteval int GetIndexByName(std::index_sequence<Is...> idxs) {
    constexpr std::array names = {Opts::GetName()...};
    for (std::size_t i = 0; i < names.size(); ++i) {
      if (names[i] == Name) {
        return i;
      }
    };
    return -1;
  }

private:
  friend class Parser<Opts...>;
  std::tuple<std::optional<typename Opts::value_type>...> values_;
};

} // namespace optica
