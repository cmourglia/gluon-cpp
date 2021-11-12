#include <gluon/lang/parser.h>

#include <gluon/lang/ast.h>
#include <gluon/lang/grammar.h>
#include <gluon/lang/lexer.h>

#include <algorithm>
#include <iterator>

ZParser::ZParser(beard::array<ZToken> Tokens)
    : m_Tokens(std::move(Tokens))
{
}

std::unique_ptr<ZExpression> ZParser::Parse()
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
std::unique_ptr<ZExpression> ZParser::Expression()
{
    return nullptr;
}

/**
 * equality   -> comparison ( ( "!=" | "==" ) comparison )* ;
 */
std::unique_ptr<ZExpression> ZParser::Equality()
{
    auto Expr = Comparison();

    while (Match({ETokenType::Equals, ETokenType::NotEquals}))
    {
        auto Operator = ConsumedToken();
        Expr          = Make<ZBinary>(std::move(Expr), Operator, Comparison());
    }

    return Expr;
}

/**
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 */
std::unique_ptr<ZExpression> ZParser::Comparison()
{
    auto Expr = Term();

    while (Match({ETokenType::Greater, ETokenType::GreaterEquals, ETokenType::Less, ETokenType::LessEquals}))
    {
        auto Operator = ConsumedToken();
        Expr          = Make<ZBinary>(std::move(Expr), Operator, Term());
    }

    return Expr;
}

/**
 * term       -> factor ( ( "-" | "+" ) factor )* ;
 */
std::unique_ptr<ZExpression> ZParser::Term()
{
    auto Expr = Factor();

    while (Match({ETokenType::Add, ETokenType::Subtract}))
    {
        auto Operator = ConsumedToken();
        Expr          = Make<ZBinary>(std::move(Expr), Operator, Factor());
    }

    return Expr;
}

/**
 * factor     -> unary ( ( "/" | "*" ) unary )* ;
 */
std::unique_ptr<ZExpression> ZParser::Factor()
{
    auto Expr = Unary();

    while (Match({ETokenType::Divide, ETokenType::Multiply}))
    {
        auto Operator = ConsumedToken();
        Expr          = Make<ZBinary>(std::move(Expr), Operator, Unary());
    }
    return Expr;
}

/**
 * unary      -> ( "not" | "-" ) unary | primary;
 */
std::unique_ptr<ZExpression> ZParser::Unary()
{
    if (Match({ETokenType::Not, ETokenType::Subtract}))
    {
        auto Operator = ConsumedToken();
        return Make<ZUnary>(Operator, Unary());
    }

    return Primary();
}

/**
 * primary    -> NUMBER | STRING | "true" | "false" | "null" | "(" expression ")" ;
 */
std::unique_ptr<ZExpression> ZParser::Primary()
{
    // clang-format off
    if (Match({ETokenType::False}))  return Make<ZLiteral>(ZValue{false});                  // NOLINT
    if (Match({ETokenType::True}))   return Make<ZLiteral>(ZValue{true});                   // NOLINT
    if (Match({ETokenType::Null}))   return Make<ZLiteral>(ZValue{nullptr});                // NOLINT
    if (Match({ETokenType::Number})) return Make<ZLiteral>(ZValue{ConsumedToken().Number}); // NOLINT
    if (Match({ETokenType::String})) return Make<ZLiteral>(ZValue{ConsumedToken().Text});   // NOLINT
    // clang-format on

    if (Match({ETokenType::OpenParen}))
    {
        auto Expr = Expression();
        Consume(ETokenType::CloseParen, "Expecting ')' after expression.");

        return Make<ZGrouping>(std::move(Expr));
    }

    ASSERT_UNREACHABLE();
    return nullptr;
}

bool ZParser::Match(std::initializer_list<ETokenType> TokenTypes)
{
    for (ETokenType TokenType : TokenTypes)
    {
        if (Check(TokenType))
        {
            Advance();
            return true;
        }
    }

    return false;
}

bool ZParser::Check(ETokenType TokenType) const
{
    return m_Tokens[m_CurrentToken].Type == TokenType;
}

bool ZParser::Done() const
{
    return m_CurrentToken >= m_Tokens.element_count();
}

void ZParser::Advance()
{
    m_CurrentToken += 1;
}

void ZParser::Consume(ETokenType TokenType, const char* ErrorString)
{
    if (Check(TokenType))
    {
        Advance();
    }
    else
    {
        throw std::exception(ErrorString);
    }
}

ZToken ZParser::ConsumedToken() const
{
    if (m_CurrentToken > 0)
    {
        return m_Tokens[m_CurrentToken - 1];
    }

    return ZToken{};
}