#pragma once

#include <Gluon/Core/Gluon_Types.h>

#include <Beard/Array.h>

#include <string>
#include <vector>

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

	Spacing,
	EndOfLine,
	Comment,

	EndOfStream,
};

struct ZToken
{
	std::string Filename;
	u32         Column, Line;

	ETokenType  Type;
	std::string Text;
	f32         Number;
};

struct ZTokenizer
{
	std::string Filename;
	u32         Column, Line;

	const char* Stream;
};

Beard::Array<ZToken> Tokenize(const char* Buffer);
