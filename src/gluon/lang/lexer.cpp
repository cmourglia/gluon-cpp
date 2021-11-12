#include <gluon/lang/lexer.h>

#include <beard/io/io.h>

std::string ToString(ETokenType TokenType)
{
    // clang-format off
	switch (TokenType)
	{
		case ETokenType::Unknown:        return "Unknown";
		case ETokenType::OpenBrace:      return "OpenBrace";
		case ETokenType::CloseBrace:     return "CloseBrace";
		case ETokenType::OpenParen:      return "OpenParen";
		case ETokenType::CloseParen:     return "CloseParen";
		case ETokenType::OpenBracket:    return "OpenBracket";
		case ETokenType::CloseBracket:   return "CloseBracket";
		case ETokenType::Comma:          return "Comma";
		case ETokenType::Colon:          return "Colon";
		case ETokenType::Semicolon:      return "Semicolon";
		case ETokenType::Dot:            return "Dot";
		case ETokenType::Add:            return "Add";
		case ETokenType::Subtract:       return "Subtract";
		case ETokenType::Multiply:       return "Multiply";
		case ETokenType::Divide:         return "Divide";
		case ETokenType::Modulo:         return "Modulo";
		case ETokenType::Power:          return "Power";
		case ETokenType::If:             return "If";
		case ETokenType::Else:           return "Else";
		case ETokenType::While:          return "While";
		case ETokenType::For:            return "For";
		case ETokenType::Let:            return "Let";
		case ETokenType::Null:           return "Null";
		case ETokenType::True:           return "True";
		case ETokenType::False:          return "False";
		case ETokenType::And:            return "And";
		case ETokenType::Or:             return "Or";
		case ETokenType::Not:            return "Not";
		case ETokenType::Equals:         return "Equals";
		case ETokenType::NotEquals:      return "NotEquals";
		case ETokenType::Greater:        return "Greater";
		case ETokenType::GreaterEquals:  return "GreaterEquals";
		case ETokenType::Less:           return "Less";
		case ETokenType::LessEquals:     return "LessEquals";
		case ETokenType::Assign:         return "Assign";
		case ETokenType::AddAssign:      return "AddAssign";
		case ETokenType::SubtractAssign: return "SubtractAssign";
		case ETokenType::MultiplyAssign: return "MultiplyAssign";
		case ETokenType::DivideAssign:   return "DivideAssign";
		case ETokenType::ModuloAssign:   return "ModuloAssign";
		case ETokenType::PowerAssign:    return "PowerAssign";
		case ETokenType::Number:         return "Number";
		case ETokenType::String:         return "String";
		case ETokenType::Identifier:     return "Identifier";
		case ETokenType::Comment:        return "Comment";
		// case ETokenType::Spacing:        return "Spacing";
		case ETokenType::EndOfLine:      return "EndOfLine";
		case ETokenType::EndOfStream:    return "EndOfStream";
	}
    // clang-format on

    ASSERT_UNREACHABLE();
    return "";
}

inline bool IsEndOfLine(char Char)
{
    bool bEndOfLine = (Char == '\n');
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

ZLexer::ZLexer(const char* Filename)
    : m_Filename{Filename}
{
    m_Buffer = beard::io::read_whole_file(Filename);
    m_Stream = m_Buffer.c_str();
}

beard::array<ZToken> ZLexer::Lex()
{
    beard::array<ZToken> Tokens;

    for (;;)
    {
        ZToken Token = GetNextToken();
        Tokens.add(Token);

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

        case '"':
        case '\'':
        case '`':
            HandleString(&Token);
            break;

        case ' ':
        case '\t':
        case '\r':
            // Do nothing
            break;

        case '\n':
            m_Column = 0;
            m_Line += 1;
            Token.Type = ETokenType::EndOfLine;
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

    if (Token.Type == ETokenType::Identifier)
    {
        // Maybe it is not and identifier after all...
        if (Token.Text == "if")
        {
            Token.Type = ETokenType::If;
        }
        else if (Token.Text == "else")
        {
            Token.Type = ETokenType::Else;
        }
        else if (Token.Text == "while")
        {
            Token.Type = ETokenType::While;
        }
        else if (Token.Text == "for")
        {
            Token.Type = ETokenType::For;
        }
        else if (Token.Text == "let")
        {
            Token.Type = ETokenType::Let;
        }
        else if (Token.Text == "null")
        {
            Token.Type = ETokenType::Null;
        }
        else if (Token.Text == "true")
        {
            Token.Type = ETokenType::True;
        }
        else if (Token.Text == "false")
        {
            Token.Type = ETokenType::False;
        }
        else if (Token.Text == "and")
        {
            Token.Type = ETokenType::And;
        }
        else if (Token.Text == "or")
        {
            Token.Type = ETokenType::Or;
        }
        else if (Token.Text == "not")
        {
            Token.Type = ETokenType::Not;
        }
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
    if (NextMatches('/'))
    {
        Token->Type = ETokenType::Comment;

        while (!IsEOF() && !IsEndOfLine(m_Stream[0]))
        {
            AdvanceChars(1);
        }
    }
    else if (NextMatches('*'))
    {
        Token->Type = ETokenType::Comment;

        while ((m_Stream[0] != '\0' && m_Stream[1] != '\0') && !(m_Stream[0] == '*' && m_Stream[1] == '/'))
        {
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
    else if (NextMatches('='))
    {
        Token->Type = ETokenType::DivideAssign;
        AdvanceChars(1);
    }
    else
    {
        Token->Type = ETokenType::Divide;
    }
}

bool ZLexer::NextMatches(char Char)
{
    return !IsEOF() && m_Stream[0] == Char;
}

bool ZLexer::IsEOF()
{
    return m_Stream[0] == '\0';
}

void ZLexer::HandleGeneralCase(ZToken* Token)
{
    if (IsAlpha(m_CurrentChar) || m_CurrentChar == '_' || m_CurrentChar == '$')
    {
        // _ and $ are valid identifier starting characters
        Token->Type = ETokenType::Identifier;

        while (IsEOF() && IsValidIdentifierCharacter(m_Stream[0]))
        {
            AdvanceChars(1);
        }
    }
    else if (IsNumber(m_CurrentChar))
    {
        Token->Type   = ETokenType::Number;
        Token->Number = ParseNumber();
    }
    else
    {
        HandleOperators(Token);
    }
}

void ZLexer::HandleOperators(ZToken* Token)
{
    auto WithFollowingEqual = [&Token, this](ETokenType DefaultValue, ETokenType AssignValue)
    {
        if (NextMatches('='))
        {
            Token->Type = AssignValue;
            AdvanceChars(1);
        }
        else
        {
            Token->Type = DefaultValue;
        }
    };

    switch (m_CurrentChar)
    {
        case '=':
            WithFollowingEqual(ETokenType::Assign, ETokenType::Equals);
            break;
        case '!':
            if (NextMatches('='))
            {
                Token->Type = ETokenType::NotEquals;
                AdvanceChars(1);
            }
            else
            {
                // This is an error
                ASSERT_UNREACHABLE();
            }
        case '>':
            WithFollowingEqual(ETokenType::Greater, ETokenType::GreaterEquals);
            break;
        case '<':
            WithFollowingEqual(ETokenType::Less, ETokenType::LessEquals);
            break;
        case '+':
            WithFollowingEqual(ETokenType::Add, ETokenType::AddAssign);
            break;
        case '-':
            WithFollowingEqual(ETokenType::Subtract, ETokenType::SubtractAssign);
            break;
        case '*':
            WithFollowingEqual(ETokenType::Multiply, ETokenType::MultiplyAssign);
            break;
        case '%':
            WithFollowingEqual(ETokenType::Modulo, ETokenType::ModuloAssign);
            break;
        case '^':
            WithFollowingEqual(ETokenType::Power, ETokenType::PowerAssign);
            break;

        default:
            // This is an error
            ASSERT_UNREACHABLE();
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