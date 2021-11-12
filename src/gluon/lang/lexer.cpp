#include <gluon/lang/lexer.h>

#include <beard/io/io.h>

std::string ToString(TokenType::Enum token_type)
{
    // clang-format off
	switch (token_type)
	{
		case TokenType::kUnknown:        return "Unknown";
		case TokenType::kOpenBrace:      return "OpenBrace";
		case TokenType::kCloseBrace:     return "CloseBrace";
		case TokenType::kOpenParen:      return "OpenParen";
		case TokenType::kCloseParen:     return "CloseParen";
		case TokenType::kOpenBracket:    return "OpenBracket";
		case TokenType::kCloseBracket:   return "CloseBracket";
		case TokenType::kComma:          return "Comma";
		case TokenType::kColon:          return "Colon";
		case TokenType::kSemicolon:      return "Semicolon";
		case TokenType::kDot:            return "Dot";
		case TokenType::kAdd:            return "Add";
		case TokenType::kSubtract:       return "Subtract";
		case TokenType::kMultiply:       return "Multiply";
		case TokenType::kDivide:         return "Divide";
		case TokenType::kModulo:         return "Modulo";
		case TokenType::kPower:          return "Power";
		case TokenType::kIf:             return "If";
		case TokenType::kElse:           return "Else";
		case TokenType::kWhile:          return "While";
		case TokenType::kFor:            return "For";
		case TokenType::kLet:            return "Let";
		case TokenType::kNull:           return "Null";
		case TokenType::kTrue:           return "True";
		case TokenType::kFalse:          return "False";
		case TokenType::kAnd:            return "And";
		case TokenType::kOr:             return "Or";
		case TokenType::kNot:            return "Not";
		case TokenType::kEquals:         return "Equals";
		case TokenType::kNotEquals:      return "NotEquals";
		case TokenType::kGreater:        return "Greater";
		case TokenType::kGreaterEquals:  return "GreaterEquals";
		case TokenType::kLess:           return "Less";
		case TokenType::kLessEquals:     return "LessEquals";
		case TokenType::kAssign:         return "Assign";
		case TokenType::kAddAssign:      return "AddAssign";
		case TokenType::kSubtractAssign: return "SubtractAssign";
		case TokenType::kMultiplyAssign: return "MultiplyAssign";
		case TokenType::kDivideAssign:   return "DivideAssign";
		case TokenType::kModuloAssign:   return "ModuloAssign";
		case TokenType::kPowerAssign:    return "PowerAssign";
		case TokenType::kNumber:         return "Number";
		case TokenType::kString:         return "String";
		case TokenType::kIdentifier:     return "Identifier";
		case TokenType::kComment:        return "Comment";
		// case ETokenType::Spacing:        return "Spacing";
		case TokenType::kEOL:      return "EndOfLine";
		case TokenType::kEOF:    return "EndOfStream";
	}
    // clang-format on

    ASSERT_UNREACHABLE();
    return "";
}

inline bool IsEndOfLine(char ch)
{
    bool is_eol = (ch == '\n');
    return is_eol;
}

inline bool IsSpacing(char ch)
{
    bool is_spacing = (ch == ' ') || (ch == '\t') || (ch == '\v') || (ch == '\f');
    return is_spacing;
}

inline bool IsWhitespace(char ch)
{
    bool is_whitespace = IsEndOfLine(ch) || IsSpacing(ch);
    return is_whitespace;
}

inline bool IsAlpha(char ch)
{
    bool is_lower_case = (ch >= 'a' && ch <= 'z');
    bool is_upper_case = (ch >= 'A' && ch <= 'Z');
    bool is_alpha      = is_lower_case || is_upper_case;

    return is_alpha;
}

inline bool IsNumber(char ch)
{
    bool is_number = (ch >= '0' && ch <= '9');
    return is_number;
}

inline bool IsValidIdentifierCharacter(char ch)
{
    bool is_valid = IsAlpha(ch) || IsNumber(ch) || ch == '_';
    return is_valid;
}

Lexer::Lexer(const char* filename)
    : m_filename{filename}
{
    m_buffer = beard::io::read_whole_file(filename);
    m_stream = m_buffer.c_str();
}

beard::array<Token> Lexer::Lex()
{
    beard::array<Token> tokens;

    for (;;)
    {
        Token token = GetNextToken();
        tokens.add(token);

        if (token.token_type == TokenType::kEOF)
        {
            break;
        }
    }

    return tokens;
}

Token Lexer::GetNextToken()
{
    Token token = {
        .filename = m_filename,
        .column   = m_Column,
        .line     = m_Line,
    };

    const char* Start = m_stream;
    m_current_char    = m_stream[0];
    AdvanceChars(1);

    switch (m_current_char)
    {
        case '\0':
            token.token_type = TokenType::kEOF;
            break;

        case '{':
            token.token_type = TokenType::kOpenBrace;
            break;
        case '}':
            token.token_type = TokenType::kCloseBrace;
            break;
        case '(':
            token.token_type = TokenType::kOpenParen;
            break;
        case ')':
            token.token_type = TokenType::kCloseParen;
            break;
        case '[':
            token.token_type = TokenType::kOpenBracket;
            break;
        case ']':
            token.token_type = TokenType::kCloseBracket;
            break;
        case ',':
            token.token_type = TokenType::kComma;
            break;
        case ':':
            token.token_type = TokenType::kColon;
            break;
        case ';':
            token.token_type = TokenType::kSemicolon;
            break;
        case '.':
            token.token_type = TokenType::kDot;
            break;

        case '"':
        case '\'':
        case '`':
            HandleString(&token);
            break;

        case ' ':
        case '\t':
        case '\r':
            // Do nothing
            break;

        case '\n':
            m_Column = 0;
            m_Line += 1;
            token.token_type = TokenType::kEOL;
            break;

        default:
            HandleGeneralCase(&token);
            break;
    }

    if (token.token_type == TokenType::kString)
    {
        // Do not keep "" or ''
        token.text = std::string(Start + 1, m_stream - 1);
    }
    else
    {
        token.text = std::string(Start, m_stream);
    }

    if (token.token_type == TokenType::kIdentifier)
    {
        // Maybe it is not and identifier after all...
        if (token.text == "if")
        {
            token.token_type = TokenType::kIf;
        }
        else if (token.text == "else")
        {
            token.token_type = TokenType::kElse;
        }
        else if (token.text == "while")
        {
            token.token_type = TokenType::kWhile;
        }
        else if (token.text == "for")
        {
            token.token_type = TokenType::kFor;
        }
        else if (token.text == "let")
        {
            token.token_type = TokenType::kLet;
        }
        else if (token.text == "null")
        {
            token.token_type = TokenType::kNull;
        }
        else if (token.text == "true")
        {
            token.token_type = TokenType::kTrue;
        }
        else if (token.text == "false")
        {
            token.token_type = TokenType::kFalse;
        }
        else if (token.text == "and")
        {
            token.token_type = TokenType::kAnd;
        }
        else if (token.text == "or")
        {
            token.token_type = TokenType::kOr;
        }
        else if (token.text == "not")
        {
            token.token_type = TokenType::kNot;
        }
    }

    return token;
}

void Lexer::AdvanceChars(u32 count)
{
    m_Column += count;
    m_stream += count;
}

void Lexer::HandleString(Token* token)
{
    token->token_type = TokenType::kString;

    while (m_stream[0] != '\0' && m_stream[0] != m_current_char)
    {
        // \' should not stop the string
        if (m_stream[0] == '\\' && m_stream[1] != '\0')
        {
            AdvanceChars(1);
        }
        AdvanceChars(1);
    }
    AdvanceChars(1);
}

void Lexer::HandleSlash(Token* token)
{
    if (NextMatches('/'))
    {
        token->token_type = TokenType::kComment;

        while (!IsEOF() && !IsEndOfLine(m_stream[0]))
        {
            AdvanceChars(1);
        }
    }
    else if (NextMatches('*'))
    {
        token->token_type = TokenType::kComment;

        while ((m_stream[0] != '\0' && m_stream[1] != '\0') && !(m_stream[0] == '*' && m_stream[1] == '/'))
        {
            if (IsEndOfLine(m_stream[0]))
            {
                m_Line += 1;
            }

            AdvanceChars(1);
        }

        if (m_stream[0] == '*')
        {
            AdvanceChars(2);
        }
    }
    else if (NextMatches('='))
    {
        token->token_type = TokenType::kDivideAssign;
        AdvanceChars(1);
    }
    else
    {
        token->token_type = TokenType::kDivide;
    }
}

bool Lexer::NextMatches(char ch)
{
    return !IsEOF() && m_stream[0] == ch;
}

bool Lexer::IsEOF()
{
    return m_stream[0] == '\0';
}

void Lexer::HandleGeneralCase(Token* token)
{
    if (IsAlpha(m_current_char) || m_current_char == '_' || m_current_char == '$')
    {
        // _ and $ are valid identifier starting characters
        token->token_type = TokenType::kIdentifier;

        while (IsEOF() && IsValidIdentifierCharacter(m_stream[0]))
        {
            AdvanceChars(1);
        }
    }
    else if (IsNumber(m_current_char))
    {
        token->token_type = TokenType::kNumber;
        token->number     = ParseNumber();
    }
    else
    {
        HandleOperators(token);
    }
}

void Lexer::HandleOperators(Token* token)
{
    auto WithFollowingEqual = [&token, this](TokenType::Enum default_value, TokenType::Enum assign_value)
    {
        if (NextMatches('='))
        {
            token->token_type = assign_value;
            AdvanceChars(1);
        }
        else
        {
            token->token_type = default_value;
        }
    };

    switch (m_current_char)
    {
        case '=':
            WithFollowingEqual(TokenType::kAssign, TokenType::kEquals);
            break;
        case '!':
            if (NextMatches('='))
            {
                token->token_type = TokenType::kNotEquals;
                AdvanceChars(1);
            }
            else
            {
                // This is an error
                ASSERT_UNREACHABLE();
            }
        case '>':
            WithFollowingEqual(TokenType::kGreater, TokenType::kGreaterEquals);
            break;
        case '<':
            WithFollowingEqual(TokenType::kLess, TokenType::kLessEquals);
            break;
        case '+':
            WithFollowingEqual(TokenType::kAdd, TokenType::kAddAssign);
            break;
        case '-':
            WithFollowingEqual(TokenType::kSubtract, TokenType::kSubtractAssign);
            break;
        case '*':
            WithFollowingEqual(TokenType::kMultiply, TokenType::kMultiplyAssign);
            break;
        case '%':
            WithFollowingEqual(TokenType::kModulo, TokenType::kModuloAssign);
            break;
        case '^':
            WithFollowingEqual(TokenType::kPower, TokenType::kPowerAssign);
            break;

        default:
            // This is an error
            ASSERT_UNREACHABLE();
    }
}

f32 Lexer::ParseNumber()
{
    f32 Value = static_cast<f32>(m_current_char - '0');

    while (m_stream[0] != '\0' && IsNumber(m_stream[0]))
    {
        f32 Digit = static_cast<f32>(m_stream[0] - '0');
        Value     = 10.0f * Value + Digit;
        AdvanceChars(1);
    }

    if (m_stream[0] != '\0' && m_stream[0] == '.')
    {
        AdvanceChars(1);

        f32 Mult = 0.1f;

        while (IsNumber(m_stream[0]))
        {
            f32 Digit = static_cast<f32>(m_stream[0] - '0');
            Value     = Value + Mult * Digit;
            Mult *= 0.1f;
            AdvanceChars(1);
        }
    }

    if (m_stream[0] == 'f')
    {
        AdvanceChars(1);
    }

    return Value;
}