#pragma once

#include <beard/containers/array.h>
#include <beard/containers/hash_map.h>
#include <beard/core/macros.h>

#include <optional>
#include <string>

#include "gluon/lang/token.h"

namespace gluon::lang {

class Lexer {
 public:
  DEFAULT_COPYABLE(Lexer);

  Lexer() = delete;
  explicit Lexer(std::string_view input);

  std::optional<Token> next();

  static beard::array<Token> lex(std::string_view source);

 private:
  Token make_token(TokenType type);

  Token handle_string();
  Token handle_number();
  Token handle_identifier();
  Token handle_one_or_two_char_token(char match_ch, TokenType one_char_type,
                                     TokenType two_chars_type);

  // Skip whitespaces and comments
  void skip_empty();

  bool done();
  [[maybe_unused]] u8 advance();
  u8 peek();
  u8 peek_next();
  u8 previous();
  bool match(u8 ch);

  u32 m_start_column = 0;
  u32 m_start_line = 0;
  u32 m_column = 0;
  u32 m_line = 0;

  std::string_view m_source;
  usize m_current = 0;
  usize m_start = 0;

  beard::array<Token> m_tokens;
};

}  // namespace gluon::lang