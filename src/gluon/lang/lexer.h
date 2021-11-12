#pragma once

#include <gluon/lang/token.h>

#include <beard/containers/array.h>
#include <beard/core/macros.h>

#include <string>

class ZLexer
{
public:
    NONCOPYABLE(ZLexer);
    NONMOVEABLE(ZLexer);

    ZLexer() = delete;
    explicit ZLexer(const char* Filename);

    beard::array<ZToken> Lex();

private:
    ZToken GetNextToken();

    void AdvanceChars(u32 Count);
    void HandleString(ZToken* Token);
    void HandleSlash(ZToken* Token);
    void HandleGeneralCase(ZToken* Token);
    void HandleOperators(ZToken* Token);

    bool IsEOF();
    bool NextMatches(char Char);

    f32 ParseNumber();

    std::string m_Filename;
    u32         m_Column = 0;
    u32         m_Line   = 0;

    std::string m_Buffer;
    const char* m_Stream      = nullptr;
    char        m_CurrentChar = '\0';
};