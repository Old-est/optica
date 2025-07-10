#pragma once

#include "program_option_builder.hpp"
#include "type_parsers.hpp"
#include <iostream>
#include <print>
namespace optica {

enum class ParsingStatus {
  Ok,
  ParsedFromDefault,
  Error,
};

template <Property... Props> struct ProgramOption : Props... {
  constexpr ProgramOption(Props &&...props) noexcept
      : Props(std::forward<Props>(props))... {}

  template <typename Iterator>
  auto TryConsume(Iterator &start, Iterator end) const
      -> std::pair<ParsingStatus,
                   std::optional<decltype(this->GetValueType())>> {
    if (start == end) {
      if constexpr (HasRequiredProperty<Props...>) {
        std::println("Option '{}' is required but not found", this->name.value);
        std::exit(2);
      }
      if constexpr (HasDefaultValueProperty<Props...>) {
        return {ParsingStatus::ParsedFromDefault, this->GetDefaultValue()};
      }
      return {ParsingStatus::Error, std::nullopt};
    }
    std::string_view token = *start;
    std::string_view name_candidate;

    // 1. Пытаемся по длинному имени
    if (token.starts_with("--")) {
      name_candidate = token.substr(2);
      if (name_candidate == this->GetName()) {
        // std::println("Matched long: {}", name_candidate);
        ++start;
        // std::cout << std::boolalpha << (start == end) << '\n';
        auto res = ConsumeArgs(start, end);
        if (!res.first) {
          std::cerr << "Cant Parse Option: " << *(start - 1) << '\n';
          std::exit(2);
        }
        return {ParsingStatus::Ok, res.second};
        // return ConsumeArgs(start, end, out);
      }
    }

    // 2. Пытаемся по короткому имени
    if constexpr (HasShortNameProperty<Props...>) {
      if (token.starts_with("-")) {
        name_candidate = token.substr(1);
        if (name_candidate == this->short_name) {
          // std::println("{}", "Matched short: {}", name_candidate);
          ++start;

          auto res = ConsumeArgs(start, end);
          if (!res.first) {
            std::cerr << "Cant Parse Option: " << *(start - 1) << '\n';
            std::exit(2);
          }
          return {ParsingStatus::Ok, res.second};

          // return ConsumeArgs(start, end, out);
        }
      }
    }

    // 3. Если опция обязательная, но не найдена
    if constexpr (HasRequiredProperty<Props...>) {
      std::println("Option '{}' is required but not found", this->name.value);
      std::exit(2);
    }

    if constexpr (HasDefaultValueProperty<Props...>) {
      std::cout << this->GetDefaultValue() << '\n';
      return {ParsingStatus::ParsedFromDefault, this->GetDefaultValue()};
    }

    return {ParsingStatus::Error, std::nullopt};
  }

  template <typename Iter>
  auto ConsumeArgs(Iter &start, Iter end) const
      -> std::pair<bool, std::optional<decltype(this->GetValueType())>> {
    if constexpr (HasArityProperty<Props...>) {
      size_t count = this->GetNumberArgs(); // Например, Exact<1> → 1
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

      decltype(this->GetValueType()) res;
      bool status = ParseType<decltype(this->GetValueType())>(result, res);

      start += count;
      // out = ...;
      if (!status) {
        return {false, std::nullopt};
      }
      return {true, res};
    }
    return {false, std::nullopt};
  }
};

namespace details {
template <typename T> struct is_program_option : std::false_type {};
template <typename... Ts>
struct is_program_option<ProgramOption<Ts...>> : std::true_type {};
} // namespace details

template <typename T>
concept IsProgramOption = details::is_program_option<T>::value;

} // namespace optica
