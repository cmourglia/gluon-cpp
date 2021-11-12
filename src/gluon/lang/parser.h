#pragma once

#include <gluon/lang/grammar.h>
#include <gluon/lang/token.h>

#include <beard/containers/array.h>

#include <memory>

class Parser
{
public:
    explicit Parser(beard::array<Token> tokens);

    std::unique_ptr<Expr> Parse();

private:
    std::unique_ptr<Expr> Expression();
    std::unique_ptr<Expr> Equality();
    std::unique_ptr<Expr> Comparison();
    std::unique_ptr<Expr> Term();
    std::unique_ptr<Expr> Factor();
    std::unique_ptr<Expr> Unary();
    std::unique_ptr<Expr> Primary();

    bool  Match(std::initializer_list<TokenType::Enum> token_types);
    bool  Check(TokenType::Enum token_type) const;
    bool  Done() const;
    void  Advance();
    void  Consume(TokenType::Enum token_type, const char* err);
    Token ConsumedToken() const;

    beard::array<Token> m_tokens;
    i32                 m_current_token = 0;
};