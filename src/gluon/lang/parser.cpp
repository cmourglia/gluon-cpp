#include "gluon/lang/parser.h"

#include <fmt/format.h>

#include <algorithm>
#include <charconv>
#include <iterator>

#include "gluon/lang/fwd.h"
#include "gluon/lang/grammar.h"
#include "gluon/lang/lexer.h"

namespace gluon::lang {

Parser::Parser(Lexer lexer) {
  std::optional<Token> token;
  while (token = lexer.next()) {
    m_tokens.add(*token);
    auto tk = *token;
    fmt::print("{}: `{}`\n", to_string(tk.type), tk.lexeme);
  }
}

ExprPtr Parser::parse() {
  auto Program = expression();

  return Program;
}

/**
 * expression -> assignment ;
 * assignment -> IDENTIFIER "=" assignment | equality ;
 * equality   -> comparison ( ( "!=" | "==" ) comparison )* ;
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 * term       -> factor ( ( "-" | "+" ) factor )* ;
 * factor     -> unary ( ( "/" | "*" ) unary )* ;
 * unary      -> ( "not" | "-" ) unary | primary;
 * primary    -> NUMBER | STRING | "true" | "false" | "null" | "(" expression
 * ")" ;
 */

/**
 * expression -> assignment ;
 */
ExprPtr Parser::expression() { return assignment(); }

/**
 * assignment -> IDENTIFIER "=" assigment | equality ;
 */
ExprPtr Parser::assignment() {
  auto expr = equality();

  if (match(TokenType::Equal)) {
    auto value = assignment();
    if (expr->type() != ExprType::Variable) {
      throw std::runtime_error{"Expression is not an l-value"};
    }

    return make<AssignExpr>(std::move(expr), std::move(value));
  }

  return expr;
}

/**
 * equality   -> comparison ( ( "!=" | "==" ) comparison )* ;
 */
ExprPtr Parser::equality() {
  auto expr = comparison();

  while (match({TokenType::EqualEqual, TokenType::BangEqual})) {
    auto op = consumed_token();
    expr = make<BinaryExpr>(std::move(expr), op, comparison());
  }

  return expr;
}

/**
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 */
ExprPtr Parser::comparison() {
  auto expr = term();

  while (match({TokenType::Greater, TokenType::GreaterEqual, TokenType::Less,
                TokenType::LessEqual})) {
    auto op = consumed_token();
    expr = make<BinaryExpr>(std::move(expr), op, term());
  }

  return expr;
}

/**
 * term       -> factor ( ( "-" | "+" ) factor )* ;
 */
ExprPtr Parser::term() {
  auto expr = factor();

  while (match({TokenType::Plus, TokenType::Minus})) {
    auto op = consumed_token();
    expr = make<BinaryExpr>(std::move(expr), op, factor());
  }

  return expr;
}

/**
 * factor     -> unary ( ( "/" | "*" ) unary )* ;
 */
ExprPtr Parser::factor() {
  auto expr = unary();

  while (match({TokenType::Slash, TokenType::Star})) {
    auto op = consumed_token();
    expr = make<BinaryExpr>(std::move(expr), op, unary());
  }
  return expr;
}

/**
 * unary      -> ( "not" | "-" ) unary | primary;
 */
ExprPtr Parser::unary() {
  if (match({TokenType::Not, TokenType::Minus})) {
    auto op = consumed_token();
    return make<UnaryExpr>(op, unary());
  }

  return primary();
}

/**
 * primary    -> NUMBER | STRING | "true" | "false" | "null" | "(" expression
 * ")" ;
 */
ExprPtr Parser::primary() {
  if (match(TokenType::True)) return make<LiteralExpr>(Value{true});
  if (match(TokenType::Nil)) return make<LiteralExpr>(Value{nullptr});
  if (match(TokenType::String))
    return make<LiteralExpr>(Value{consumed_token().lexeme});

  if (match(TokenType::Number)) {
    f64 number = 0.0;
    auto lexeme = consumed_token().lexeme;
    auto num_result =
        std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), number);

    if (num_result.ec != std::errc{}) {
      // Error
      return make<LiteralExpr>(Value::kUndefined);
    }

    return make<LiteralExpr>(Value{number});  // NOLINT
  }

  if (match(TokenType::OpenParen)) {
    auto expr = expression();
    consume(TokenType::CloseParen, "Expecting ')' after expression.");

    return make<GroupingExpr>(std::move(expr));
  }

  ASSERT_UNREACHABLE();
  return nullptr;
}

bool Parser::match(TokenType token_type) { return match({token_type}); }

bool Parser::match(std::initializer_list<TokenType> token_types) {
  for (auto token_type : token_types) {
    if (check(token_type)) {
      advance();
      return true;
    }
  }

  return false;
}

bool Parser::check(TokenType token_type) const {
  if (done())
    return false;

  return m_tokens[m_current_token].type == token_type;
}

bool Parser::done() const {
  return m_current_token >= m_tokens.element_count();
}

void Parser::advance() { m_current_token += 1; }

void Parser::consume(TokenType token_type, const char* ErrorString) {
  if (check(token_type)) {
    advance();
  } else {
    throw std::runtime_error{ErrorString};
  }
}

Token Parser::consumed_token() const {
  if (m_current_token > 0) {
    return m_tokens[m_current_token - 1];
  }

  return Token{};
}

ExprPtr parse(Lexer lexer) {
  Parser parser{lexer};
  return parser.parse();
}

}  // namespace gluon::lang