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
	ZIdentifier,

	Comment,

	Spacing,
	EndOfLine,
	EndOfStream,
};

struct ZToken
{
};

class ZLexer
{
public:
};