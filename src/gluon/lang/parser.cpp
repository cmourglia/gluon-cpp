#include <gluon/lang/parser.h>

#include <gluon/lang/ast.h>
#include <gluon/lang/grammar.h>
#include <gluon/lang/lexer.h>

#include <algorithm>
#include <iterator>

namespace parser {
Parser::Parser(beard::array<Token> tokens) : m_tokens(std::move(tokens)) {}

ExprPtr Parser::Parse() {
  /*auto Program = Make<ZBinary>(Make<ZUnary>(ZToken{.Type =
     ETokenType::Subtract, .Text = "-"}, Make<ZLiteral>(ZValue{123})),
                               ZToken{.Type = ETokenType::Multiply, .Text =
     "*"}, Make<ZGrouping>(Make<ZLiteral>(ZValue{45.67})));*/

  auto Program = Expression();

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
ExprPtr Parser::Expression() {
  return Assignment();
}

/**
 * assignment -> IDENTIFIER "=" assigment | equality ;
 */
ExprPtr Parser::Assignment() {
  auto expr = Equality();

  if (Match(TokenType::kAssign)) {
    auto value = Assignment();
    if (expr->type() != ExprType::kVariable) {
      throw std::exception("Expression is not an l-value");
    }

    return Make<AssignExpr>(std::move(expr), std::move(value));
  }

  return expr;
}

/**
 * equality   -> comparison ( ( "!=" | "==" ) comparison )* ;
 */
ExprPtr Parser::Equality() {
  auto expr = Comparison();

  while (Match({TokenType::kEquals, TokenType::kNotEquals})) {
    auto op = ConsumedToken();
    expr = Make<BinaryExpr>(std::move(expr), op, Comparison());
  }

  return expr;
}

/**
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 */
ExprPtr Parser::Comparison() {
  auto expr = Term();

  while (Match({TokenType::kGreater, TokenType::kGreaterEquals,
                TokenType::kLess, TokenType::kLessEquals})) {
    auto op = ConsumedToken();
    expr = Make<BinaryExpr>(std::move(expr), op, Term());
  }

  return expr;
}

/**
 * term       -> factor ( ( "-" | "+" ) factor )* ;
 */
ExprPtr Parser::Term() {
  auto expr = Factor();

  while (Match({TokenType::kAdd, TokenType::kSubtract})) {
    auto op = ConsumedToken();
    expr = Make<BinaryExpr>(std::move(expr), op, Factor());
  }

  return expr;
}

/**
 * factor     -> unary ( ( "/" | "*" ) unary )* ;
 */
ExprPtr Parser::Factor() {
  auto expr = Unary();

  while (Match({TokenType::kDivide, TokenType::kMultiply})) {
    auto op = ConsumedToken();
    expr = Make<BinaryExpr>(std::move(expr), op, Unary());
  }
  return expr;
}

/**
 * unary      -> ( "not" | "-" ) unary | primary;
 */
ExprPtr Parser::Unary() {
  if (Match({TokenType::kNot, TokenType::kSubtract})) {
    auto op = ConsumedToken();
    return Make<UnaryExpr>(op, Unary());
  }

  return Primary();
}

/**
 * primary    -> NUMBER | STRING | "true" | "false" | "null" | "(" expression
 * ")" ;
 */
ExprPtr Parser::Primary() {
  // clang-format off
    if (Match(TokenType::kFalse))  return Make<LiteralExpr>(Value{false});                // NOLINT
    if (Match(TokenType::kTrue))   return Make<LiteralExpr>(Value{true});                 // NOLINT
    if (Match(TokenType::kNull))   return Make<LiteralExpr>(Value{nullptr});                    // NOLINT
    if (Match(TokenType::kNumber)) return Make<LiteralExpr>(Value{ConsumedToken().value.value().AsNumber()});     // NOLINT
    if (Match(TokenType::kString)) return Make<LiteralExpr>(Value{ConsumedToken().lexeme}); // NOLINT
  // clang-format on

  if (Match(TokenType::kOpenParen)) {
    auto expr = Expression();
    Consume(TokenType::kCloseParen, "Expecting ')' after expression.");

    return Make<GroupingExpr>(std::move(expr));
  }

  ASSERT_UNREACHABLE();
  return nullptr;
}

bool Parser::Match(TokenType::Enum token_type) {
  return Match({token_type});
}

bool Parser::Match(std::initializer_list<TokenType::Enum> token_types) {
  for (auto token_type : token_types) {
    if (Check(token_type)) {
      Advance();
      return true;
    }
  }

  return false;
}

bool Parser::Check(TokenType::Enum token_type) const {
  return m_tokens[m_current_token].token_type == token_type;
}

bool Parser::Done() const {
  return m_current_token >= m_tokens.element_count();
}

void Parser::Advance() {
  m_current_token += 1;
}

void Parser::Consume(TokenType::Enum token_type, const char* ErrorString) {
  if (Check(token_type)) {
    Advance();
  } else {
    throw std::exception(ErrorString);
  }
}

Token Parser::ConsumedToken() const {
  if (m_current_token > 0) {
    return m_tokens[m_current_token - 1];
  }

  return Token{};
}

ExprPtr Parse(beard::array<Token> tokens) {
  Parser parser{tokens};
  return parser.Parse();
}
}  // namespace parser