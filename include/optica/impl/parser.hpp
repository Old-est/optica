#pragma once

#include <iostream>
#include <optional>
#include <ranges>
#include <string_view>
#include <tuple>
#include <vector>

#include "fixed_string.hpp"
#include "program_option.hpp"
namespace optica {

template <FixedString Name, typename... Options>
concept HasName = (... || (Options::name == Name));

template <typename... Options> class Parser;

enum class Result {
  OK,
  Error,
};

template <typename... Options> class ParserResult {
public:
  template <FixedString Name>
    requires HasName<Name, Options...>
  constexpr auto Get() const noexcept {
    constexpr std::size_t index =
        GetIndexByName<Name>(std::index_sequence_for<Options...>{});
    return std::get<index>(values_);
  }

private:
  template <FixedString Name, std::size_t... Is>
  static constexpr std::size_t
  GetIndexByName(std::index_sequence<Is...> /*idxs*/) {
    std::size_t index = -1;
    [[maybe_unused]] bool found =
        ((Options::name == Name ? (index = Is, true) : false) || ...);
    return index;
  }

private:
  friend Parser<Options...>;

  Result result_{Result::OK};
  std::tuple<std::optional<typename Options::value_type>...> values_;
};

template <typename... Options> class Parser {
public:
  template <typename... Opts>
  constexpr Parser(Opts &&...opts) noexcept
      : types_(ToProgramOptionT<Opts>(std::forward<Opts>(opts))...) {}

  auto Parse(int argc, char **argv) const -> ParserResult<Options...>;
  auto Parse(std::string_view data) const -> ParserResult<Options...>;

private:
  std::tuple<Options...> types_;
};

template <typename... Opts>
Parser(Opts &&...) -> Parser<ToProgramOptionT<Opts>...>;

template <typename... Options>
auto Parser<Options...>::Parse(int argc, char **argv) const
    -> ParserResult<Options...> {
  ParserResult<Options...> result;
  std::vector<std::string_view> argv_data(argv + 1, argv + argc);

  auto parse_one = [&](auto I) {
    constexpr size_t index = decltype(I)::value;
    const auto &opt = std::get<index>(types_);
    using return_type =
        std::decay_t<decltype(std::get<index>(result.values_))>::value_type;
    auto parsed = opt.template TryParse<return_type>(argv_data);
    if (parsed) {
      std::get<index>(result.values_) = std::move(parsed);
    }
  };

  [&]<std::size_t... Is>(std::index_sequence<Is...>) {
    (parse_one(std::integral_constant<std::size_t, Is>{}), ...);
  }(std::index_sequence_for<Options...>{});
  return result;
}

template <typename... Options>
auto Parser<Options...>::Parse(std::string_view data) const
    -> ParserResult<Options...> {
  ParserResult<Options...> result;
  auto res = std::ranges::views::split(data, ' ') |
             std::views::transform([](auto &&r) {
               return std::string_view{r.begin(), r.end()};
             }) |
             std::ranges::to<std::vector<std::string_view>>();
  auto end = res.end();
  for (auto start = res.begin(); start != end;) {

    constexpr auto N = sizeof...(Options);
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      bool matched = ((std::get<I>(types_).TryConsume(
                          start, end, std::get<I>(result.values_))) ||
                      ...);
      if (!matched) {
        std::cerr << "Unknown Argument: " << *start << '\n';
        std::exit(1);
      }
    }(std::make_index_sequence<N>{});
  }

  return result;
}

} // namespace optica
