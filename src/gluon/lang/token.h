#pragma once

#include <beard/containers/hash_map.h>
#include <beard/core/macros.h>

#include <charconv>
#include <optional>
#include <string_view>

namespace gluon::lang {

enum class TokenType : u8 {
  Unknown,

  OpenBrace,
  CloseBrace,
  OpenParen,
  CloseParen,
  OpenBracket,
  CloseBracket,
  Comma,
  Colon,
  Semicolon,
  Dot,

  Equal,
  EqualEqual,
  BangEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  Plus,
  PlusEqual,
  Minus,
  MinusEqual,
  Star,
  StarEqual,
  Slash,
  SlashEqual,
  Percent,
  PercentEqual,

  // Literals
  Number,
  String,
  Identifier,

  // Keywords
  And,
  Class,
  Else,
  False,
  For,
  Fn,
  If,
  Let,
  Nil,
  Not,
  Or,
  Return,
  Super,
  Switch,
  This,
  True,
  While,
  Xor,

  // Temp
  Print,

  EOL,
};

static const beard::hash_map<std::string_view, TokenType> kKeywords = {
    {"and", TokenType::And},     {"class", TokenType::Class},
    {"else", TokenType::Else},   {"false", TokenType::False},
    {"for", TokenType::For},     {"fn", TokenType::Fn},
    {"if", TokenType::If},       {"let", TokenType::Let},
    {"nil", TokenType::Nil},     {"not", TokenType::Not},
    {"or", TokenType::Or},       {"return", TokenType::Return},
    {"super", TokenType::Super}, {"switch", TokenType::Switch},
    {"this", TokenType::This},   {"true", TokenType::True},
    {"while", TokenType::While}, {"xor", TokenType::Xor},
    {"print", TokenType::Print},
};

struct Token {
  std::string_view lexeme;
  u32 column, line;

  TokenType type;
};

const char* to_string(TokenType token_type);

template <typename T>
std::optional<T> to_number(const Token& token) {
  T result;

  if (token.type != TokenType::Number) {
    return std::nullopt;
  }

  auto cvt_result = std::from_chars(
      token.lexeme.data(), token.lexeme.data() + token.lexeme.size(), result);

  if (cvt_result.ec != std::errc{}) {
    return std::nullopt;
  }
  return result;
}

}  // namespace gluon::lang
