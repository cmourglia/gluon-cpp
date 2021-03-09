#pragma once

#include "types.h"

#include <string>
#include <vector>

enum TokenType
{
	Token_Unknown,

	Token_OpenBrace,
	Token_CloseBrace,
	Token_OpenParen,
	Token_CloseParen,
	Token_OpenBracket,
	Token_CloseBracket,
	Token_Comma,
	Token_Colon,
	Token_Semicolon,
	Token_Dot,

	Token_Plus,
	Token_Minus,
	Token_Asterisk,
	Token_Slash,

	Token_Number,
	Token_String,
	Token_Identifier,

	Token_Spacing,
	Token_EndOfLine,
	Token_Comment,

	Token_EndOfStream,
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
