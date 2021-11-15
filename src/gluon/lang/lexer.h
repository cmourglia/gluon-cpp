#pragma once

#include <gluon/lang/token.h>

#include <beard/containers/array.h>
#include <beard/containers/hash_map.h>
#include <beard/core/macros.h>

#include <string>

namespace lexer
{
class Lexer
{
public:
    NONCOPYABLE(Lexer);
    NONMOVEABLE(Lexer);

    Lexer() = delete;
    explicit Lexer(const char* filename);

    beard::array<Token> Lex();

private:
    void ReadNextToken();
    void AddToken(TokenType::Enum token_type, beard::optional<Value> value = beard::optional<Value>{});

    void HandleMultilineComment();
    void HandleString();
    void HandleNumber();
    void HandleIdentifier();

    bool Done();
    u32  Advance();
    u32  Peek();
    u32  PeekNext();
    u32  Previous();
    bool MatchChar(u32 ch);

    f32 ParseNumber();

    std::string m_filename;
    u32         m_column = 0;
    u32         m_line   = 0;

    std::u32string m_source;
    usize          m_current;
    usize          m_start;

    beard::array<Token>               m_tokens;
    beard::string_hash_map<TokenType> m_keywords;
};

beard::array<Token> Lex(const char* filename);
}