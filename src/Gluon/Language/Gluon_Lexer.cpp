#include <Gluon/Language/Gluon_Lexer.h>

#include <Beard/IO.h>

std::string ToString(ETokenType TokenType)
{
	switch (TokenType)
	{
		case ETokenType::Unknown:
			return "Unknown";
		case ETokenType::OpenBrace:
			return "OpenBrace";
		case ETokenType::CloseBrace:
			return "CloseBrace";
		case ETokenType::OpenParen:
			return "OpenParen";
		case ETokenType::CloseParen:
			return "CloseParen";
		case ETokenType::OpenBracket:
			return "OpenBracket";
		case ETokenType::CloseBracket:
			return "CloseBracket";
		case ETokenType::Comma:
			return "Comma";
		case ETokenType::Colon:
			return "Colon";
		case ETokenType::Semicolon:
			return "Semicolon";
		case ETokenType::Dot:
			return "Dot";
		case ETokenType::Plus:
			return "Plus";
		case ETokenType::Minus:
			return "Minus";
		case ETokenType::Asterisk:
			return "Asterisk";
		case ETokenType::Slash:
			return "Slash";
		case ETokenType::Number:
			return "Number";
		case ETokenType::String:
			return "String";
		case ETokenType::Identifier:
			return "Identifier";
		case ETokenType::Comment:
			return "Comment";
		case ETokenType::Spacing:
			return "Spacing";
		case ETokenType::EndOfLine:
			return "EndOfLine";
		case ETokenType::EndOfStream:
			return "EndOfStream";
	}

	return "";
}

inline bool IsEndOfLine(char Char)
{
	bool bEndOfLine = (Char == '\r') || (Char == '\n');
	return bEndOfLine;
}

inline bool IsSpacing(char Char)
{
	bool bSpacing = (Char == ' ') || (Char == '\t') || (Char == '\v') || (Char == '\f');
	return bSpacing;
}

inline bool IsWhitespace(char Char)
{
	bool bWhitespace = IsEndOfLine(Char) || IsSpacing(Char);
	return bWhitespace;
}

inline bool IsAlpha(char Char)
{
	bool bLowerCase = (Char >= 'a' && Char <= 'z');
	bool bUpperCase = (Char >= 'A' && Char <= 'Z');
	bool bAlpha     = bLowerCase || bUpperCase;

	return bAlpha;
}

inline bool IsNumber(char Char)
{
	bool bNumber = (Char >= '0' && Char <= '9');
	return bNumber;
}

inline bool IsValidIdentifierCharacter(char Char)
{
	bool bValid = IsAlpha(Char) || IsNumber(Char) || Char == '_';
	return bValid;
}

Beard::Array<ZToken> ZLexer::Lex(const char* Filename)
{
	ZLexer Lexer{Filename};

	return Lexer.Lex();
}

ZLexer::ZLexer(const char* Filename)
    : m_Filename{Filename}
{
	m_Buffer = Beard::IO::ReadWholeFile(Filename);
	m_Stream = m_Buffer.c_str();
}

Beard::Array<ZToken> ZLexer::Lex()
{
	Beard::Array<ZToken> Tokens;

	for (;;)
	{
		ZToken Token = GetNextToken();
		Tokens.Add(Token);

		if (Token.Type == ETokenType::EndOfStream)
		{
			break;
		}
	}

	return Tokens;
}

ZToken ZLexer::GetNextToken()
{
	ZToken Token = {
	    .Filename = m_Filename,
	    .Column   = m_Column,
	    .Line     = m_Line,
	};

	const char* Start = m_Stream;
	m_CurrentChar     = m_Stream[0];
	AdvanceChars(1);

	switch (m_CurrentChar)
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
		case '\'':
		case '`':
			HandleString(&Token);
			break;

		case '/':
			HandleSlash(&Token);
			break;

		default:
			HandleGeneralCase(&Token);
			break;
	}

	if (Token.Type == ETokenType::String)
	{
		// Do not keep "" or ''
		Token.Text = std::string(Start + 1, m_Stream - 1);
	}
	else
	{
		Token.Text = std::string(Start, m_Stream);
	}

	return Token;
}

void ZLexer::AdvanceChars(u32 Count)
{
	m_Column += Count;
	m_Stream += Count;
}

void ZLexer::HandleString(ZToken* Token)
{
	Token->Type = ETokenType::String;

	while (m_Stream[0] != '\0' && m_Stream[0] != m_CurrentChar)
	{
		// \' should not stop the string
		if (m_Stream[0] == '\\' && m_Stream[1] != '\0')
		{
			AdvanceChars(1);
		}
		AdvanceChars(1);
	}
	AdvanceChars(1);
}

void ZLexer::HandleSlash(ZToken* Token)
{
	if (m_Stream[0] == '/')
	{
		Token->Type = ETokenType::Comment;

		while (m_Stream[0] != '\0' && !IsEndOfLine(m_Stream[0]))
		{
			AdvanceChars(1);
		}
	}
	else if (m_Stream[0] == '*')
	{
		Token->Type = ETokenType::Comment;

		while ((m_Stream[0] != '\0' && m_Stream[1] != '\0') && !(m_Stream[0] == '*' && m_Stream[1] == '/'))
		{
			if ((m_Stream[0] == '\r' && m_Stream[1] == '\n') || (m_Stream[0] == '\n' && m_Stream[1] == '\r'))
			{
				AdvanceChars(1);
			}

			if (IsEndOfLine(m_Stream[0]))
			{
				m_Line += 1;
			}

			AdvanceChars(1);
		}

		if (m_Stream[0] == '*')
		{
			AdvanceChars(2);
		}
	}
	else
	{
		Token->Type = ETokenType::Slash;
	}
}

void ZLexer::HandleGeneralCase(ZToken* Token)
{
	if (IsEndOfLine(m_CurrentChar))
	{
		Token->Type = ETokenType::EndOfLine;

		if ((m_CurrentChar == '\r' && m_Stream[0] == '\n') || (m_CurrentChar == '\n' && m_Stream[0] == '\r'))
		{
			AdvanceChars(1);
		}

		m_Column = 0;
		m_Line += 1;
	}
	else if (IsSpacing(m_CurrentChar))
	{
		Token->Type = ETokenType::Spacing;
		while (m_Stream[0] != '\0' && IsSpacing(m_Stream[0]))
		{
			AdvanceChars(1);
		}
	}
	else if (IsAlpha(m_CurrentChar))
	{
		Token->Type = ETokenType::Identifier;

		while (m_Stream[0] != '\0' && IsValidIdentifierCharacter(m_Stream[0]))
		{
			AdvanceChars(1);
		}
	}
	else if (IsNumber(m_CurrentChar))
	{
		Token->Type   = ETokenType::Number;
		Token->Number = ParseNumber();
	}
}

f32 ZLexer::ParseNumber()
{
	f32 Value = static_cast<f32>(m_CurrentChar - '0');

	while (m_Stream[0] != '\0' && IsNumber(m_Stream[0]))
	{
		f32 Digit = static_cast<f32>(m_Stream[0] - '0');
		Value     = 10.0f * Value + Digit;
		AdvanceChars(1);
	}

	if (m_Stream[0] != '\0' && m_Stream[0] == '.')
	{
		AdvanceChars(1);

		f32 Mult = 0.1f;

		while (IsNumber(m_Stream[0]))
		{
			f32 Digit = static_cast<f32>(m_Stream[0] - '0');
			Value     = Value + Mult * Digit;
			Mult *= 0.1f;
			AdvanceChars(1);
		}
	}

	if (m_Stream[0] == 'f')
	{
		AdvanceChars(1);
	}

	return Value;
}