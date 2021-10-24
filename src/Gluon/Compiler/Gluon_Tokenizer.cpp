#include <Gluon/Compiler/Gluon_Tokenizer.h>

#include <loguru.hpp>

inline void AdvanceChars(ZTokenizer* Tokenizer, u32 Count)
{
	Tokenizer->Column += Count;
	Tokenizer->Stream += Count;
}

inline bool IsEndOfLine(char c)
{
	const bool Result = (c == '\r') || (c == '\n');
	return Result;
}

inline bool IsSpacing(char c)
{
	const bool Result = (c == ' ') || (c == '\t') || (c == '\v') || (c == '\f');
	return Result;
}

inline bool IsWhitespace(char c)
{
	const bool Result = IsEndOfLine(c) || IsSpacing(c);
	return Result;
}

inline bool IsAlpha(char c)
{
	const bool Result = (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');
	return Result;
}

inline bool IsNumber(char c)
{
	const bool Result = (c >= '0' && c <= '9');
	return Result;
}

inline ZToken GetToken(ZTokenizer* Tokenizer)
{
	ZToken Token = {
	    .Filename = Tokenizer->Filename,
	    .Column   = Tokenizer->Column,
	    .Line     = Tokenizer->Line,
	};

	const char* Start = Tokenizer->Stream;
	const char  Char  = Tokenizer->Stream[0];
	AdvanceChars(Tokenizer, 1);

	switch (Char)
	{
		case '\0':
			Token.Type = ETokenType::EndOfStream;
			break;

		case '{':
			Token.Type = ETokenType::OpenBrace;
			break;
		case '}':
			Token.Type = ETokenType::CloseBrace;
			break;
		case '(':
			Token.Type = ETokenType::OpenParen;
			break;
		case ')':
			Token.Type = ETokenType::CloseParen;
			break;
		case '[':
			Token.Type = ETokenType::OpenBracket;
			break;
		case ']':
			Token.Type = ETokenType::CloseBracket;
			break;
		case ',':
			Token.Type = ETokenType::Comma;
			break;
		case ':':
			Token.Type = ETokenType::Colon;
			break;
		case ';':
			Token.Type = ETokenType::Semicolon;
			break;
		case '.':
			Token.Type = ETokenType::Dot;
			break;
		case '+':
			Token.Type = ETokenType::Plus;
			break;
		case '-':
			Token.Type = ETokenType::Minus;
			break;
		case '*':
			Token.Type = ETokenType::Asterisk;
			break;

		case '"':
		{
			Token.Type = ETokenType::String;

			while (Tokenizer->Stream[0] && Tokenizer->Stream[0] != '"')
			{
				// \" should not stop the string
				if (Tokenizer->Stream[0] == '\\' && Tokenizer->Stream[1])
				{
					AdvanceChars(Tokenizer, 1);
				}
				AdvanceChars(Tokenizer, 1);
			}
			AdvanceChars(Tokenizer, 1);
		}
		break;

		case '\'':
		{
			Token.Type = ETokenType::String;

			while (Tokenizer->Stream[0] && Tokenizer->Stream[0] != '\'')
			{
				// \' should not stop the string
				if (Tokenizer->Stream[0] == '\\' && Tokenizer->Stream[1])
				{
					AdvanceChars(Tokenizer, 1);
				}
				AdvanceChars(Tokenizer, 1);
			}
			AdvanceChars(Tokenizer, 1);
		}
		break;

		case '/':
		{
			if (Tokenizer->Stream[0] == '/')
			{
				Token.Type = ETokenType::Comment;

				while (Tokenizer->Stream[0] && !IsEndOfLine(Tokenizer->Stream[0]))
				{
					AdvanceChars(Tokenizer, 1);
				}
			}
			else if (Tokenizer->Stream[0] == '*')
			{
				Token.Type = ETokenType::Comment;

				while ((Tokenizer->Stream[0] && Tokenizer->Stream[1]) &&
				       !(Tokenizer->Stream[0] == '*' && Tokenizer->Stream[1] == '/'))
				{
					if ((Tokenizer->Stream[0] == '\r' && Tokenizer->Stream[1] == '\n') ||
					    (Tokenizer->Stream[0] == '\n' && Tokenizer->Stream[1] == '\r'))
					{
						AdvanceChars(Tokenizer, 1);
					}

					if (IsEndOfLine(Tokenizer->Stream[0]))
					{
						++Tokenizer->Line;
					}

					AdvanceChars(Tokenizer, 1);
				}

				if (Tokenizer->Stream[0] == '*')
				{
					AdvanceChars(Tokenizer, 2);
				}
			}
			else
			{
				Token.Type = ETokenType::Slash;
			}
		}
		break;

		default:
		{
			if (IsEndOfLine(Char))
			{
				Token.Type = ETokenType::EndOfLine;

				if ((Char == '\r' && Tokenizer->Stream[0] == '\n') || (Char == '\n' && Tokenizer->Stream[0] == '\r'))
				{
					AdvanceChars(Tokenizer, 1);
				}

				Tokenizer->Column = 0;
				Tokenizer->Line += 1;
			}
			else if (IsSpacing(Char))
			{
				Token.Type = ETokenType::Spacing;
				while (Tokenizer->Stream[0] && IsSpacing(Tokenizer->Stream[0]))
				{
					AdvanceChars(Tokenizer, 1);
				}
			}
			else if (IsAlpha(Char))
			{
				Token.Type = ETokenType::ZIdentifier;

				while (Tokenizer->Stream[0] &&
				       (IsAlpha(Tokenizer->Stream[0]) || IsNumber(Tokenizer->Stream[0]) || Tokenizer->Stream[0] == '_'))
				{
					AdvanceChars(Tokenizer, 1);
				}
			}
			else if (IsNumber(Char))
			{
				Token.Type = ETokenType::Number;

				f32 Value = (f32)(Char - '0');

				while (IsNumber(Tokenizer->Stream[0]))
				{
					f32 Digit = (f32)(Tokenizer->Stream[0] - '0');
					Value     = 10.0f * Value + Digit;
					AdvanceChars(Tokenizer, 1);
				}

				if (Tokenizer->Stream[0] == '.')
				{
					AdvanceChars(Tokenizer, 1);

					f32 Mult = 0.1f;

					while (IsNumber(Tokenizer->Stream[0]))
					{
						f32 Digit = (f32)(Tokenizer->Stream[0] - '0');
						Value     = Value + Mult * Digit;
						Mult *= 0.1f;
						AdvanceChars(Tokenizer, 1);
					}
				}

				Token.Number = Value;

				if (Tokenizer->Stream[0] == 'f')
				{
					AdvanceChars(Tokenizer, 1);
				}
			}
		}
	}

	if (Token.Type == ETokenType::String)
	{
		// Do not keep "" or ''
		Token.Text = std::string(Start + 1, Tokenizer->Stream - 1);
	}
	else
	{
		Token.Text = std::string(Start, Tokenizer->Stream);
	}

	return Token;
}

Beard::Array<ZToken> Tokenize(const char* Buffer)
{
	Beard::Array<ZToken> Tokens;

	ZTokenizer Tokenizer = {
	    // .Filename = Filename,
	    .Column = 0,
	    .Line   = 0,
	    .Stream = Buffer,
	};

	for (;;)
	{
		ZToken Token = GetToken(&Tokenizer);

		Tokens.Add(Token);

		if (Token.Type == ETokenType::EndOfStream)
		{
			break;
		}
	}

	return Tokens;
}