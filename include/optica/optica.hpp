#pragma once

#include <iostream>
#include <optional>
#include <print>
#include <string_view>
#include <tuple>
#include <vector>

namespace optica {

template <typename T>
concept HasName = requires {
  { T::name } -> std::convertible_to<std::string_view>;
} && (T::name.size() > 0);

template <typename T>
concept HasRequired = requires {
  { T::required } -> std::convertible_to<bool>;
};

template <typename T>
concept HasShortName = requires {
  { T::short_name } -> std::convertible_to<std::string_view>;
};

struct One {};
template <std::size_t N> struct Exact {
  static constexpr std::size_t value = N;
};

template <typename T>
concept HasArity = requires { typename T::arity_type; };

template <typename T>
concept ValidArity =
    std::same_as<T, One> || (requires {
      T::value;
    } && std::integral_constant<std::size_t, T::value>::value == T::value);

template <typename Value, typename Derived> struct Option {

public:
  using value_type = Value;
  using arity_type = decltype([] {
    if constexpr (HasArity<Derived>) {
      if constexpr (ValidArity<typename Derived::arity_type>) {
        return typename Derived::arity_type{};
      } else {
        return One{};
      }
    } else {
      return One{};
    }
  });

  static constexpr std::string_view get_name() {
    static_assert(HasName<Derived>, "Option must have non-empty name");
    return Derived::name;
  }

  static constexpr std::string_view get_short_name() {
    if constexpr (HasShortName<Derived>) {
      return Derived::short_name;
    } else {
      return "";
    }
  }

  static constexpr bool get_required() {
    if constexpr (HasRequired<Derived>) {
      return Derived::required;
    } else {
      return false;
    }
  }
};

template <typename T> bool ParseValue(std::string_view str, T &out);

template <typename... Ts> struct type_list {};

template <typename T, typename... Rest>
constexpr bool contains = (std::same_as<T, Rest> || ...);

template <typename List> constexpr bool is_unique = true;

template <typename Head, typename... Tail>
constexpr bool is_unique<type_list<Head, Tail...>> =
    !contains<Head, Tail...> && is_unique<type_list<Tail...>>;

template <typename... Ts>
concept UniqueTypes = is_unique<type_list<Ts...>>;

template <typename... Options>
  requires UniqueTypes<Options...>
class Parser {
public:
  Parser() = default;

  void Parse(int argc, char **argv) {
    std::vector<std::string_view> argumets(argv + 1, argv + argc);

    parse_all_impl(argumets, std::index_sequence_for<Options...>{});
  }

  template <typename Opt> const auto &Get() const {
    return std::get<std::optional<typename Opt::value_type>>(values_);
  }

  void PrintMeta() const { (print_single<Options>(), ...); }

private:
  template <typename Opt> static void print_single() {
    std::println("Option name:    {}", Opt::get_name());
    std::println("    short name:    {}", Opt::get_short_name());
    std::println("    required:    {}", Opt::get_required());
  }

  template <size_t... Is>
  void parse_all_impl(std::vector<std::string_view> &arguments,
                      std::index_sequence<Is...>) {
    (parse_single_option<std::tuple_element_t<Is, std::tuple<Options...>>>(
         std::get<Is>(values_), arguments),
     ...);
  }

  template <typename Opt>
  void parse_single_option(std::optional<typename Opt::value_type> &val_opt,
                           std::vector<std::string_view> &argumets) {
    std::string target_name = "--" + std::string(Opt::get_name());
    auto result = std::find(argumets.begin(), argumets.end(), target_name);
    if (result == argumets.end()) {
      if (Opt::get_required()) {
        std::cerr << "Missing required option: --" << Opt::get_name() << '\n';
        std::exit(1); // завершить программу с кодом ошибки
      }
    } else {
      auto end = std::find_if(result + 1, argumets.end(),
                              [](auto a) { return a.starts_with("-"); });

      std::string parsing_string;
      auto rng = std::ranges::subrange(result + 1, end);
      for (auto &&part : rng) {
        if (!parsing_string.empty()) {
          parsing_string += ' ';
        }
        parsing_string.append(part.data(), part.size());
      }

      typename Opt::value_type val;
      if (ParseValue(parsing_string, val)) {
        val_opt = val;
      } else {
        std::cerr << "Invalid value for option: " << Opt::get_name()
                  << " provided: " << parsing_string << '\n';
        std::exit(2);
      }
    }
  }

private:
  std::tuple<std::optional<typename Options::value_type>...> values_;
};

template <> inline bool ParseValue(std::string_view str, int &out) {
  auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), out);
  return ec == std::errc();
}

} // namespace optica
