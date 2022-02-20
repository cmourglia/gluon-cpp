#pragma once

#include "gluon/lang/grammar.h"
#include "gluon/lang/lexer.h"
#include "gluon/lang/token.h"

#include <beard/containers/array.h>

#include <memory>

namespace gluon::lang {

class Parser {
 public:
  explicit Parser(Lexer lexer);

  ExprPtr parse();

 private:
  ExprPtr expression();
  ExprPtr assignment();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr primary();

  [[nodiscard]] bool match(TokenType token_type);
  [[nodiscard]] bool match(std::initializer_list<TokenType> token_types);
  [[nodiscard]] bool check(TokenType token_type) const;
  [[nodiscard]] bool done() const;
  void advance();
  void consume(TokenType token_type, const char* err);
  [[nodiscard]] Token consumed_token() const;

  beard::array<Token> m_tokens;
  i32 m_current_token = 0;
};

ExprPtr parse(Lexer lexer);

}  // namespace gluon::lang