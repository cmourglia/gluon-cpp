#include "tokenizer.h"

#include "utils.h"

#include <loguru.hpp>

#include <assert.h>

#include <Windows.h>

inline void AdvanceChars(Tokenizer* tokenizer, u32 count)
{
	tokenizer->column += count;
	tokenizer->stream += count;
}

inline bool IsEndOfLine(char c)
{
	const bool result = (c == '\r') || (c == '\n');
	return result;
}

inline bool IsSpacing(char c)
{
	const bool result = (c == ' ') || (c == '\t') || (c == '\v') || (c == '\f');
	return result;
}

inline bool IsWhitespace(char c)
{
	const bool result = IsEndOfLine(c) || IsSpacing(c);
	return result;
}

inline bool IsAlpha(char c)
{
	const bool result = (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');
	return result;
}

inline bool IsNumber(char c)
{
	const bool result = (c >= '0' && c <= '9');
	return result;
}

inline Token GetToken(Tokenizer* tokenizer)
{
	Token token = {
	    .filename = tokenizer->filename,
	    .column   = tokenizer->column,
	    .line     = tokenizer->line,
	};

	const char* start = tokenizer->stream;
	const char  c     = tokenizer->stream[0];
	AdvanceChars(tokenizer, 1);

	switch (c)
	{
		case '\0':
			token.type = Token_EndOfStream;
			break;

		case '{':
			token.type = Token_OpenBrace;
			break;
		case '}':
			token.type = Token_CloseBrace;
			break;
		case '(':
			token.type = Token_OpenParen;
			break;
		case ')':
			token.type = Token_CloseParen;
			break;
		case '[':
			token.type = Token_OpenBracket;
			break;
		case ']':
			token.type = Token_CloseBracket;
			break;
		case ',':
			token.type = Token_Comma;
			break;
		case ':':
			token.type = Token_Colon;
			break;
		case ';':
			token.type = Token_Semicolon;
			break;
		case '.':
			token.type = Token_Dot;
			break;
		case '+':
			token.type = Token_Plus;
			break;
		case '-':
			token.type = Token_Minus;
			break;
		case '*':
			token.type = Token_Asterisk;
			break;

		case '"':
		{
			token.type = Token_String;

			while (tokenizer->stream[0] && tokenizer->stream[0] != '"')
			{
				// \" should not stop the string
				if (tokenizer->stream[0] == '\\' && tokenizer->stream[1])
				{
					AdvanceChars(tokenizer, 1);
				}
				AdvanceChars(tokenizer, 1);
			}
			AdvanceChars(tokenizer, 1);
		}
		break;

		case '\'':
		{
			token.type = Token_String;

			while (tokenizer->stream[0] && tokenizer->stream[0] != '\'')
			{
				// \' should not stop the string
				if (tokenizer->stream[0] == '\\' && tokenizer->stream[1])
				{
					AdvanceChars(tokenizer, 1);
				}
				AdvanceChars(tokenizer, 1);
			}
			AdvanceChars(tokenizer, 1);
		}
		break;

		case '/':
		{
			if (tokenizer->stream[0] == '/')
			{
				token.type = Token_Comment;

				while (tokenizer->stream[0] && !IsEndOfLine(tokenizer->stream[0]))
				{
					AdvanceChars(tokenizer, 1);
				}
			}
			else if (tokenizer->stream[0] == '*')
			{
				token.type = Token_Comment;

				while ((tokenizer->stream[0] && tokenizer->stream[1]) &&
				       !(tokenizer->stream[0] == '*' && tokenizer->stream[1] == '/'))
				{
					if ((tokenizer->stream[0] == '\r' && tokenizer->stream[1] == '\n') ||
					    (tokenizer->stream[0] == '\n' && tokenizer->stream[1] == '\r'))
					{
						AdvanceChars(tokenizer, 1);
					}

					if (IsEndOfLine(tokenizer->stream[0]))
					{
						++tokenizer->line;
					}

					AdvanceChars(tokenizer, 1);
				}

				if (tokenizer->stream[0] == '*')
				{
					AdvanceChars(tokenizer, 2);
				}
			}
			else
			{
				token.type = Token_Slash;
			}
		}
		break;

		default:
		{
			if (IsEndOfLine(c))
			{
				token.type = Token_EndOfLine;

				if ((c == '\r' && tokenizer->stream[0] == '\n') || (c == '\n' && tokenizer->stream[0] == '\r'))
				{
					AdvanceChars(tokenizer, 1);
				}

				tokenizer->column = 0;
				tokenizer->line += 1;
			}
			else if (IsSpacing(c))
			{
				token.type = Token_Spacing;
				while (tokenizer->stream[0] && IsSpacing(tokenizer->stream[0]))
				{
					AdvanceChars(tokenizer, 1);
				}
			}
			else if (IsAlpha(c))
			{
				token.type = Token_Identifier;

				while (tokenizer->stream[0] &&
				       (IsAlpha(tokenizer->stream[0]) || IsNumber(tokenizer->stream[0]) || tokenizer->stream[0] == '_'))
				{
					AdvanceChars(tokenizer, 1);
				}
			}
			else if (IsNumber(c))
			{
				token.type = Token_Number;

				f32 value = (f32)(c - '0');

				while (IsNumber(tokenizer->stream[0]))
				{
					f32 digit = (f32)(tokenizer->stream[0] - '0');
					value     = 10.0f * value + digit;
					AdvanceChars(tokenizer, 1);
				}

				if (tokenizer->stream[0] == '.')
				{
					AdvanceChars(tokenizer, 1);

					f32 mult = 0.1f;

					while (IsNumber(tokenizer->stream[0]))
					{
						f32 digit = (f32)(tokenizer->stream[0] - '0');
						value     = value + mult * digit;
						mult *= 0.1f;
						AdvanceChars(tokenizer, 1);
					}
				}

				token.number = value;

				if (tokenizer->stream[0] == 'f')
				{
					AdvanceChars(tokenizer, 1);
				}
			}
		}
	}

	token.text = std::string(start, tokenizer->stream);

	return token;
}

std::vector<Token> Tokenize(std::string_view buffer)
{
	std::vector<Token> tokens;

	bool done = false;

	const char* t = buffer.data();

	Tokenizer tokenizer = {
	    // .filename = filename,
	    .column = 0,
	    .line   = 0,
	    .stream = buffer.data(),
	};

	for (;;)
	{
		Token token = GetToken(&tokenizer);

		tokens.push_back(token);

		if (token.type == Token_EndOfStream)
		{
			break;
		}
	}

	return tokens;
}