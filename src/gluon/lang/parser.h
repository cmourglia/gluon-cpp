#pragma once

#include <gluon/lang/grammar.h>
#include <gluon/lang/token.h>

#include <beard/containers/array.h>

#include <memory>

class ZParser
{
public:
    explicit ZParser(beard::array<ZToken> Tokens);

    std::unique_ptr<ZExpression> Parse();

private:
    std::unique_ptr<ZExpression> Expression();
    std::unique_ptr<ZExpression> Equality();
    std::unique_ptr<ZExpression> Comparison();
    std::unique_ptr<ZExpression> Term();
    std::unique_ptr<ZExpression> Factor();
    std::unique_ptr<ZExpression> Unary();
    std::unique_ptr<ZExpression> Primary();

    bool   Match(std::initializer_list<ETokenType> TokenTypes);
    bool   Check(ETokenType TokenType) const;
    bool   Done() const;
    void   Advance();
    void   Consume(ETokenType TokenType, const char* ErrorString);
    ZToken ConsumedToken() const;

    beard::array<ZToken> m_Tokens;
    i32                  m_CurrentToken = 0;
};