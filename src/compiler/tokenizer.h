#pragma once

#include "types.h"

#include <string>
#include <vector>

enum class TokenType
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

	Spacing,
	EndOfLine,
	Comment,

	EndOfStream,
};

struct Token
{
	std::string filename;
	u32         column, line;

	TokenType   type;
	std::string text;
	f32         number;
};

struct Tokenizer
{
	std::string filename;
	u32         column, line;

	const char* stream;
};

std::vector<Token> Tokenize(std::string_view buffer);
