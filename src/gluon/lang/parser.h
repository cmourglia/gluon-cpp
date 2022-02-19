#pragma once

#include <gluon/lang/grammar.h>
#include <gluon/lang/token.h>

#include <beard/containers/array.h>

#include <memory>

namespace parser {
class Parser {
 public:
  explicit Parser(beard::array<Token> tokens);

  ExprPtr Parse();

 private:
  ExprPtr Expression();
  ExprPtr Assignment();
  ExprPtr Equality();
  ExprPtr Comparison();
  ExprPtr Term();
  ExprPtr Factor();
  ExprPtr Unary();
  ExprPtr Primary();

  [[nodiscard]] bool Match(TokenType::Enum token_type);
  [[nodiscard]] bool Match(std::initializer_list<TokenType::Enum> token_types);
  [[nodiscard]] bool Check(TokenType::Enum token_type) const;
  [[nodiscard]] bool Done() const;
  void Advance();
  void Consume(TokenType::Enum token_type, const char* err);
  [[nodiscard]] Token ConsumedToken() const;

  beard::array<Token> m_tokens;
  i32 m_current_token = 0;
};

ExprPtr Parse(beard::array<Token> tokens);
}  // namespace parser