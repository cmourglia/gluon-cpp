#pragma once

#include <Beard/Array.h>
#include <Beard/Macros.h>

#include <string>

enum class ETokenType
{
	Unknown,

	OpenBrace,
	CloseBrace,
	OpenParen,
	CloseParen,
	OpenBracket,
	CloseBracket,

	Comma,
	Colon,
	Semicolon,
	Dot,

	Plus,
	Minus,
	Asterisk,
	Slash,

	Number,
	String,
	Identifier,

	Comment,

	Spacing,
	EndOfLine,
	EndOfStream,
};

std::string ToString(ETokenType TokenType);

struct ZToken
{
	std::string Filename;
	u32         Column, Line;

	ETokenType  Type;
	std::string Text;
	f32         Number;
};

class ZLexer
{
public:
	NONCOPYABLE(ZLexer);
	NONMOVEABLE(ZLexer);

	ZLexer() = delete;

	static Beard::Array<ZToken> Lex(const char* Filename);

private:
	ZLexer(const char* Filename);

	Beard::Array<ZToken> Lex();
	ZToken               GetNextToken();

	void AdvanceChars(u32 Count);
	void HandleString(ZToken* Token);
	void HandleSlash(ZToken* Token);
	void HandleGeneralCase(ZToken* Token);

	f32 ParseNumber();

	std::string m_Filename;
	u32         m_Column = 0;
	u32         m_Line   = 0;

	std::string m_Buffer;
	const char* m_Stream      = nullptr;
	char        m_CurrentChar = '\0';
};