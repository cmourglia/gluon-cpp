#pragma once

#include <gluon/lang/token.h>

#include <beard/containers/array.h>
#include <beard/core/macros.h>

#include <string>

class Lexer
{
public:
    NONCOPYABLE(Lexer);
    NONMOVEABLE(Lexer);

    Lexer() = delete;
    explicit Lexer(const char* filename);

    beard::array<Token> Lex();

private:
    Token GetNextToken();

    void AdvanceChars(u32 count);
    void HandleString(Token* token);
    void HandleSlash(Token* token);
    void HandleGeneralCase(Token* token);
    void HandleOperators(Token* default_value);

    bool IsEOF();
    bool NextMatches(char ch);

    f32 ParseNumber();

    std::string m_filename;
    u32         m_Column = 0;
    u32         m_Line   = 0;

    std::string m_buffer;
    const char* m_stream       = nullptr;
    char        m_current_char = '\0';
};