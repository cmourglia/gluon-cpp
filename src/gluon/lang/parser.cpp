#include <gluon/lang/parser.h>

#include <gluon/lang/ast.h>
#include <gluon/lang/grammar.h>
#include <gluon/lang/lexer.h>

#include <algorithm>
#include <iterator>

Parser::Parser(beard::array<Token> tokens)
    : m_tokens(std::move(tokens))
{
}

std::unique_ptr<Expr> Parser::Parse()
{
    /*auto Program = Make<ZBinary>(Make<ZUnary>(ZToken{.Type = ETokenType::Subtract, .Text = "-"},
                                              Make<ZLiteral>(ZValue{123})),
                                 ZToken{.Type = ETokenType::Multiply, .Text = "*"},
                                 Make<ZGrouping>(Make<ZLiteral>(ZValue{45.67})));*/

    auto Program = Expression();

    return Program;
}

/**
 * expression -> equality ;
 * equality   -> comparison ( ( "!=" | "==" ) comparison )* ;
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 * term       -> factor ( ( "-" | "+" ) factor )* ;
 * factor     -> unary ( ( "/" | "*" ) unary )* ;
 * unary      -> ( "not" | "-" ) unary | primary;
 * primary    -> NUMBER | STRING | "true" | "false" | "null" | "(" expression ")" ;
 */

/**
 * expression -> equality ;
 */
std::unique_ptr<Expr> Parser::Expression()
{
    return nullptr;
}

/**
 * equality   -> comparison ( ( "!=" | "==" ) comparison )* ;
 */
std::unique_ptr<Expr> Parser::Equality()
{
    auto Expr = Comparison();

    while (Match({TokenType::kEquals, TokenType::kNotEquals}))
    {
        auto Operator = ConsumedToken();
        Expr          = Make<BinaryExpr>(std::move(Expr), Operator, Comparison());
    }

    return Expr;
}

/**
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 */
std::unique_ptr<Expr> Parser::Comparison()
{
    auto Expr = Term();

    while (Match({TokenType::kGreater, TokenType::kGreaterEquals, TokenType::kLess, TokenType::kLessEquals}))
    {
        auto Operator = ConsumedToken();
        Expr          = Make<BinaryExpr>(std::move(Expr), Operator, Term());
    }

    return Expr;
}

/**
 * term       -> factor ( ( "-" | "+" ) factor )* ;
 */
std::unique_ptr<Expr> Parser::Term()
{
    auto Expr = Factor();

    while (Match({TokenType::kAdd, TokenType::kSubtract}))
    {
        auto Operator = ConsumedToken();
        Expr          = Make<BinaryExpr>(std::move(Expr), Operator, Factor());
    }

    return Expr;
}

/**
 * factor     -> unary ( ( "/" | "*" ) unary )* ;
 */
std::unique_ptr<Expr> Parser::Factor()
{
    auto Expr = Unary();

    while (Match({TokenType::kDivide, TokenType::kMultiply}))
    {
        auto Operator = ConsumedToken();
        Expr          = Make<BinaryExpr>(std::move(Expr), Operator, Unary());
    }
    return Expr;
}

/**
 * unary      -> ( "not" | "-" ) unary | primary;
 */
std::unique_ptr<Expr> Parser::Unary()
{
    if (Match({TokenType::kNot, TokenType::kSubtract}))
    {
        auto Operator = ConsumedToken();
        return Make<UnaryExpr>(Operator, Unary());
    }

    return Primary();
}

/**
 * primary    -> NUMBER | STRING | "true" | "false" | "null" | "(" expression ")" ;
 */
std::unique_ptr<Expr> Parser::Primary()
{
    // clang-format off
    if (Match({TokenType::kFalse}))  return Make<LiteralExpr>(Value{false});                // NOLINT
    if (Match({TokenType::kTrue}))   return Make<LiteralExpr>(Value{true});                 // NOLINT
    if (Match({TokenType::kNull}))   return Make<LiteralExpr>(Value{nullptr});                    // NOLINT
    if (Match({TokenType::kNumber})) return Make<LiteralExpr>(Value{ConsumedToken().number});     // NOLINT
    if (Match({TokenType::kString})) return Make<LiteralExpr>(Value{ConsumedToken().text}); // NOLINT
    // clang-format on

    if (Match({TokenType::kOpenParen}))
    {
        auto Expr = Expression();
        Consume(TokenType::kCloseParen, "Expecting ')' after expression.");

        return Make<GroupingExpr>(std::move(Expr));
    }

    ASSERT_UNREACHABLE();
    return nullptr;
}

bool Parser::Match(std::initializer_list<TokenType::Enum> token_types)
{
    for (auto token_type : token_types)
    {
        if (Check(token_type))
        {
            Advance();
            return true;
        }
    }

    return false;
}

bool Parser::Check(TokenType::Enum token_type) const
{
    return m_tokens[m_current_token].token_type == token_type;
}

bool Parser::Done() const
{
    return m_current_token >= m_tokens.element_count();
}

void Parser::Advance()
{
    m_current_token += 1;
}

void Parser::Consume(TokenType::Enum token_type, const char* ErrorString)
{
    if (Check(token_type))
    {
        Advance();
    }
    else
    {
        throw std::exception(ErrorString);
    }
}

Token Parser::ConsumedToken() const
{
    if (m_current_token > 0)
    {
        return m_tokens[m_current_token - 1];
    }

    return Token{};
}