#include <Gluon/Compiler/Tokenizer.h>

#include <loguru.hpp>

inline void advance_chars(Tokenizer* tokenizer, u32 count)
{
	tokenizer->column += count;
	tokenizer->stream += count;
}

inline bool is_eol(char c)
{
	const bool result = (c == '\r') || (c == '\n');
	return result;
}

inline bool is_spacing(char c)
{
	const bool result = (c == ' ') || (c == '\t') || (c == '\v') || (c == '\f');
	return result;
}

inline bool is_whitespace(char c)
{
	const bool result = is_eol(c) || is_spacing(c);
	return result;
}

inline bool is_alpha(char c)
{
	const bool result = (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');
	return result;
}

inline bool is_number(char c)
{
	const bool result = (c >= '0' && c <= '9');
	return result;
}

inline Token get_token(Tokenizer* tokenizer)
{
	Token token = {
	    .filename = tokenizer->filename,
	    .column   = tokenizer->column,
	    .line     = tokenizer->line,
	};

	const char* start = tokenizer->stream;
	const char  c     = tokenizer->stream[0];
	advance_chars(tokenizer, 1);

	switch (c)
	{
		case '\0':
			token.type = TokenType::EndOfStream;
			break;

		case '{':
			token.type = TokenType::OpenBrace;
			break;
		case '}':
			token.type = TokenType::CloseBrace;
			break;
		case '(':
			token.type = TokenType::OpenParen;
			break;
		case ')':
			token.type = TokenType::CloseParen;
			break;
		case '[':
			token.type = TokenType::OpenBracket;
			break;
		case ']':
			token.type = TokenType::CloseBracket;
			break;
		case ',':
			token.type = TokenType::Comma;
			break;
		case ':':
			token.type = TokenType::Colon;
			break;
		case ';':
			token.type = TokenType::Semicolon;
			break;
		case '.':
			token.type = TokenType::Dot;
			break;
		case '+':
			token.type = TokenType::Plus;
			break;
		case '-':
			token.type = TokenType::Minus;
			break;
		case '*':
			token.type = TokenType::Asterisk;
			break;

		case '"':
		{
			token.type = TokenType::String;

			while (tokenizer->stream[0] && tokenizer->stream[0] != '"')
			{
				// \" should not stop the string
				if (tokenizer->stream[0] == '\\' && tokenizer->stream[1])
				{
					advance_chars(tokenizer, 1);
				}
				advance_chars(tokenizer, 1);
			}
			advance_chars(tokenizer, 1);
		}
		break;

		case '\'':
		{
			token.type = TokenType::String;

			while (tokenizer->stream[0] && tokenizer->stream[0] != '\'')
			{
				// \' should not stop the string
				if (tokenizer->stream[0] == '\\' && tokenizer->stream[1])
				{
					advance_chars(tokenizer, 1);
				}
				advance_chars(tokenizer, 1);
			}
			advance_chars(tokenizer, 1);
		}
		break;

		case '/':
		{
			if (tokenizer->stream[0] == '/')
			{
				token.type = TokenType::Comment;

				while (tokenizer->stream[0] && !is_eol(tokenizer->stream[0]))
				{
					advance_chars(tokenizer, 1);
				}
			}
			else if (tokenizer->stream[0] == '*')
			{
				token.type = TokenType::Comment;

				while ((tokenizer->stream[0] && tokenizer->stream[1]) &&
				       !(tokenizer->stream[0] == '*' && tokenizer->stream[1] == '/'))
				{
					if ((tokenizer->stream[0] == '\r' && tokenizer->stream[1] == '\n') ||
					    (tokenizer->stream[0] == '\n' && tokenizer->stream[1] == '\r'))
					{
						advance_chars(tokenizer, 1);
					}

					if (is_eol(tokenizer->stream[0]))
					{
						++tokenizer->line;
					}

					advance_chars(tokenizer, 1);
				}

				if (tokenizer->stream[0] == '*')
				{
					advance_chars(tokenizer, 2);
				}
			}
			else
			{
				token.type = TokenType::Slash;
			}
		}
		break;

		default:
		{
			if (is_eol(c))
			{
				token.type = TokenType::EndOfLine;

				if ((c == '\r' && tokenizer->stream[0] == '\n') ||
				    (c == '\n' && tokenizer->stream[0] == '\r'))
				{
					advance_chars(tokenizer, 1);
				}

				tokenizer->column = 0;
				tokenizer->line += 1;
			}
			else if (is_spacing(c))
			{
				token.type = TokenType::Spacing;
				while (tokenizer->stream[0] && is_spacing(tokenizer->stream[0]))
				{
					advance_chars(tokenizer, 1);
				}
			}
			else if (is_alpha(c))
			{
				token.type = TokenType::Identifier;

				while (tokenizer->stream[0] &&
				       (is_alpha(tokenizer->stream[0]) || is_number(tokenizer->stream[0]) ||
				        tokenizer->stream[0] == '_'))
				{
					advance_chars(tokenizer, 1);
				}
			}
			else if (is_number(c))
			{
				token.type = TokenType::Number;

				f32 value = (f32)(c - '0');

				while (is_number(tokenizer->stream[0]))
				{
					f32 digit = (f32)(tokenizer->stream[0] - '0');
					value     = 10.0f * value + digit;
					advance_chars(tokenizer, 1);
				}

				if (tokenizer->stream[0] == '.')
				{
					advance_chars(tokenizer, 1);

					f32 mult = 0.1f;

					while (is_number(tokenizer->stream[0]))
					{
						f32 digit = (f32)(tokenizer->stream[0] - '0');
						value     = value + mult * digit;
						mult *= 0.1f;
						advance_chars(tokenizer, 1);
					}
				}

				token.number = value;

				if (tokenizer->stream[0] == 'f')
				{
					advance_chars(tokenizer, 1);
				}
			}
		}
	}

	if (token.type == TokenType::String)
	{
		// Do not keep "" or ''
		token.text = std::string(start + 1, tokenizer->stream - 1);
	}
	else
	{
		token.text = std::string(start, tokenizer->stream);
	}

	return token;
}

std::vector<Token> tokenize(const char* buffer)
{
	std::vector<Token> tokens;

	Tokenizer tokenizer = {
	    // .filename = filename,
	    .column = 0,
	    .line   = 0,
	    .stream = buffer,
	};

	for (;;)
	{
		Token token = get_token(&tokenizer);

		tokens.push_back(token);

		if (token.type == TokenType::EndOfStream)
		{
			break;
		}
	}

	return tokens;
}