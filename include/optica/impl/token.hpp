#pragma once

#include <cstddef>
#include <format>
#include <print>
#include <string_view>
namespace optica {

class TokenIterator;

/**
 * @class Token
 * @brief This class represents token extracted from string
 *
 * In terms of optica there are few types of tokens
 * \ref TokenType
 *
 */
struct Token {
  /**
   * @brief Token types
   */
  enum class TokenType { None = 0, Word, LongName, ShortName, CompoundName };

  /**
   * @brief Default constructor
   */
  constexpr Token() noexcept = default;
  /**
   * @brief Constructs Token from std::string_view and token type
   *
   * @param data actual token data
   * @param type type of a token
   */
  constexpr Token(std::string_view data, TokenType type) noexcept
      : data_(data), type_(type),
        token_size_(std::count_if(data_.begin(), data_.end(),
                                  [](auto symbol) { return symbol == ','; }) +
                    1) {}

  /**
   * @brief Checks if two tokens are equal
   *
   * @param other Other Token
   * @return bool
   */
  constexpr bool operator==(const Token &other) const noexcept {
    return data_ == other.data_ && type_ == other.type_;
  }

  /**
   * @brief Get access to token data
   *
   * @return std::string_view view on data
   */
  [[nodiscard]] constexpr std::string_view GetTokenData() const noexcept {
    return data_;
  }
  /**
   * @brief Get Token Size
   *
   * @return std::size_t token size
   *
   * @remark
   * Token size shows how many units can be extracted from token
   * for general arguments it equals to 1, but for compound tokens it can be
   * more. In terms of CMD string compound token is something like {1, 2, 3}.
   * After parsing this string you'll get Token with size of 3
   */
  [[nodiscard]] constexpr std::size_t GetTokenSize() const noexcept {
    return token_size_;
  }

private:
  friend std::formatter<Token>;
  friend TokenIterator;

private:
  std::string_view data_;
  TokenType type_{TokenType::None};
  std::size_t token_size_{};
};

constexpr std::string_view to_string(Token::TokenType type) {
  using enum Token::TokenType;
  switch (type) {
  case None:
    return "None";
  case Word:
    return "Word";
  case LongName:
    return "LongName";
  case ShortName:
    return "ShortName";
  case CompoundName:
    return "CompoundName";
  default:
    return "Unknown";
  }
}

/**
 * @class TokenIterator
 * @brief Iterator on string_view
 *
 * This Iterator sequiently produces
 * new tokens extracted from std::string_view
 *
 */
class TokenIterator {
public:
  using value_type = Token;
  using reference = Token &;
  using pointer = Token *;
  using difference_type = std::ptrdiff_t;

  /**
   * @brief Default constructor
   */
  constexpr TokenIterator() noexcept = default;
  /**
   * @brief Constructs Iterator
   *
   * @param data Start of a of the sequence of chars
   * @param end End
   */
  constexpr TokenIterator(const char *data, const char *end) noexcept
      : current_(data), end_(end) {
    parse_token();
  }

  /**
   * @brief Prefix increment operator
   *
   * @return TokenIterator with new \ref Token inside
   */
  constexpr TokenIterator &operator++() noexcept {
    parse_token();
    return *this;
  };
  /**
   * @brief Postfix increment operator
   *
   * @return Returns old TokenIterator but gets new one
   */
  constexpr TokenIterator operator++(int) noexcept { return *this; };

  /**
   * @brief Checks if 2 TokenIterators are equal
   *
   * @param other Other TokenIterator
   */
  constexpr bool operator==(const TokenIterator &other) const noexcept {
    return current_ == other.current_ && end_ == other.end_ &&
           current_token_ == other.current_token_;
  }

  /**
   * @brief Get access to created Token
   *
   * @return Token
   */
  constexpr const Token &operator*() const noexcept { return current_token_; }

private:
  constexpr void parse_token() noexcept {
    // пропускаем пробелы
    auto current_copy = current_;
    while (current_ != end_ &&
           (*current_ == ' ' || *current_ == ',' || *current_ == '=')) {
      ++current_;
    }
    if (current_ == end_) {
      current_token_ = Token{};
      return;
    }

    if (current_copy == current_ &&
        current_token_.type_ == Token::TokenType::ShortName) {
      current_token_ =
          Token(std::string_view(current_copy, 1), Token::TokenType::ShortName);
      ++current_;
      return;
    }

    Token::TokenType type = Token::TokenType::Word;
    const char *start = current_;

    if (*start == '{') {
      type = Token::TokenType::CompoundName;
    }
    if (*start == '-') {
      type = Token::TokenType::ShortName;
    }
    if (*(start + 1) == '-') {
      type = Token::TokenType::LongName;
    }

    if (type == Token::TokenType::CompoundName) {
      while (current_ != end_ && *current_ != '}') {
        ++current_;
      }
      ++current_;
    } else if (type == Token::TokenType::ShortName) {
      current_ += 2;
    } else {
      while (current_ != end_ && *current_ != ' ' && *current_ != ',' &&
             *current_ != '=') {
        ++current_;
      }
    }
    if (type == Token::TokenType::LongName) {
      current_token_ = Token(std::string_view(start + 2, current_), type);
      return;
    } else if (type == Token::TokenType::ShortName) {
      current_token_ = Token(std::string_view(start + 1, current_), type);
      return;
    }
    current_token_ = Token(
        std::string_view(start, static_cast<std::size_t>(current_ - start)),
        type);
  }

private:
  const char *current_{};
  const char *end_{};
  Token current_token_;
};

static_assert(std::forward_iterator<TokenIterator>, "kek");

/**
 * @class Tokenizer
 * @brief Class that creates Tokenized context
 *
 */
class Tokenizer {
public:
  /**
   * @brief Constructs Tokenizer
   *
   * @param data std::string_view with sequence of chars
   */
  constexpr Tokenizer(std::string_view data) noexcept : data_string_(data) {}

  /**
   * @brief Get TokenIterator that holds first Token
   *
   * @return TokenIterator with first token
   */
  constexpr TokenIterator begin() const noexcept {
    return TokenIterator{data_string_.begin(), data_string_.end()};
  }
  /**
   * @brief Get End of Tokenizer
   *
   * @return Sentinel
   */
  constexpr TokenIterator end() const noexcept {
    return TokenIterator{data_string_.end(), data_string_.end()};
  }

private:
  std::string_view data_string_;
};

} // namespace optica

template <> struct std::formatter<optica::Token> {
  constexpr auto parse(std::format_parse_context &ctx) {
    return ctx.begin(); // без кастомных спецификаторов
  }

  template <typename FormatContext>
  auto format(const optica::Token &token, FormatContext &ctx) const {
    return std::format_to(ctx.out(), "Token(data=\"{}\", type={}, size={})",
                          token.data_, to_string(token.type_),
                          token.token_size_);
  }
};
