#pragma once

#include <gluon/core/types.h>

#include <beard/containers/array.h>

#include <string>
#include <vector>

struct TokenType {
  enum Enum {
    kUnknown,

    kOpenBrace,
    kCloseBrace,
    kOpenParen,
    kCloseParen,
    kOpenBracket,
    kCloseBracket,
    kComma,
    kColon,
    kSemicolon,
    kDot,

    kPlus,
    kMinus,
    kAsterisk,
    kSlash,

    kNumber,
    kString,
    kIdentifier,

    kSpacing,
    kEOL,
    kComment,

    kEOF,
  };
};

struct Token {
  std::string filename;
  u32 column, line;

  TokenType::Enum token_type;
  std::string text;
  f32 number;
};

struct Tokenizer {
  std::string filename;
  u32 column, line;

  const char* stream;
};

beard::array<Token> Tokenize(const char* buffer);
