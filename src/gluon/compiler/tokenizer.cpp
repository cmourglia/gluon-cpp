#include <gluon/compiler/tokenizer.h>

#include <loguru.hpp>

inline void AdvanceChars(Tokenizer* Tokenizer, u32 Count) {
  Tokenizer->column += Count;
  Tokenizer->stream += Count;
}

inline bool IsEndOfLine(char c) {
  const bool Result = (c == '\r') || (c == '\n');
  return Result;
}

inline bool IsSpacing(char c) {
  const bool Result = (c == ' ') || (c == '\t') || (c == '\v') || (c == '\f');
  return Result;
}

inline bool IsWhitespace(char c) {
  const bool Result = IsEndOfLine(c) || IsSpacing(c);
  return Result;
}

inline bool IsAlpha(char c) {
  const bool Result = (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');
  return Result;
}

inline bool IsNumber(char c) {
  const bool Result = (c >= '0' && c <= '9');
  return Result;
}

inline Token GetToken(Tokenizer* Tokenizer) {
  Token Token = {
      .filename = Tokenizer->filename,
      .column = Tokenizer->column,
      .line = Tokenizer->line,
  };

  const char* Start = Tokenizer->stream;
  const char Char = Tokenizer->stream[0];
  AdvanceChars(Tokenizer, 1);

  switch (Char) {
    case '\0':
      Token.token_type = TokenType::kEOF;
      break;

    case '{':
      Token.token_type = TokenType::kOpenBrace;
      break;
    case '}':
      Token.token_type = TokenType::kCloseBrace;
      break;
    case '(':
      Token.token_type = TokenType::kOpenParen;
      break;
    case ')':
      Token.token_type = TokenType::kCloseParen;
      break;
    case '[':
      Token.token_type = TokenType::kOpenBracket;
      break;
    case ']':
      Token.token_type = TokenType::kCloseBracket;
      break;
    case ',':
      Token.token_type = TokenType::kComma;
      break;
    case ':':
      Token.token_type = TokenType::kColon;
      break;
    case ';':
      Token.token_type = TokenType::kSemicolon;
      break;
    case '.':
      Token.token_type = TokenType::kDot;
      break;
    case '+':
      Token.token_type = TokenType::kPlus;
      break;
    case '-':
      Token.token_type = TokenType::kMinus;
      break;
    case '*':
      Token.token_type = TokenType::kAsterisk;
      break;

    case '"': {
      Token.token_type = TokenType::kString;

      while (Tokenizer->stream[0] && Tokenizer->stream[0] != '"') {
        // \" should not stop the string
        if (Tokenizer->stream[0] == '\\' && Tokenizer->stream[1]) {
          AdvanceChars(Tokenizer, 1);
        }
        AdvanceChars(Tokenizer, 1);
      }
      AdvanceChars(Tokenizer, 1);
    } break;

    case '\'': {
      Token.token_type = TokenType::kString;

      while (Tokenizer->stream[0] && Tokenizer->stream[0] != '\'') {
        // \' should not stop the string
        if (Tokenizer->stream[0] == '\\' && Tokenizer->stream[1]) {
          AdvanceChars(Tokenizer, 1);
        }
        AdvanceChars(Tokenizer, 1);
      }
      AdvanceChars(Tokenizer, 1);
    } break;

    case '/': {
      if (Tokenizer->stream[0] == '/') {
        Token.token_type = TokenType::kComment;

        while (Tokenizer->stream[0] && !IsEndOfLine(Tokenizer->stream[0])) {
          AdvanceChars(Tokenizer, 1);
        }
      } else if (Tokenizer->stream[0] == '*') {
        Token.token_type = TokenType::kComment;

        while ((Tokenizer->stream[0] && Tokenizer->stream[1]) &&
               !(Tokenizer->stream[0] == '*' && Tokenizer->stream[1] == '/')) {
          if ((Tokenizer->stream[0] == '\r' && Tokenizer->stream[1] == '\n') ||
              (Tokenizer->stream[0] == '\n' && Tokenizer->stream[1] == '\r')) {
            AdvanceChars(Tokenizer, 1);
          }

          if (IsEndOfLine(Tokenizer->stream[0])) {
            ++Tokenizer->line;
          }

          AdvanceChars(Tokenizer, 1);
        }

        if (Tokenizer->stream[0] == '*') {
          AdvanceChars(Tokenizer, 2);
        }
      } else {
        Token.token_type = TokenType::kSlash;
      }
    } break;

    default: {
      if (IsEndOfLine(Char)) {
        Token.token_type = TokenType::kEOL;

        if ((Char == '\r' && Tokenizer->stream[0] == '\n') ||
            (Char == '\n' && Tokenizer->stream[0] == '\r')) {
          AdvanceChars(Tokenizer, 1);
        }

        Tokenizer->column = 0;
        Tokenizer->line += 1;
      } else if (IsSpacing(Char)) {
        Token.token_type = TokenType::kSpacing;
        while (Tokenizer->stream[0] && IsSpacing(Tokenizer->stream[0])) {
          AdvanceChars(Tokenizer, 1);
        }
      } else if (IsAlpha(Char)) {
        Token.token_type = TokenType::kIdentifier;

        while (Tokenizer->stream[0] && (IsAlpha(Tokenizer->stream[0]) ||
                                        IsNumber(Tokenizer->stream[0]) ||
                                        Tokenizer->stream[0] == '_')) {
          AdvanceChars(Tokenizer, 1);
        }
      } else if (IsNumber(Char)) {
        Token.token_type = TokenType::kNumber;

        f32 Value = (f32)(Char - '0');

        while (IsNumber(Tokenizer->stream[0])) {
          f32 Digit = (f32)(Tokenizer->stream[0] - '0');
          Value = 10.0f * Value + Digit;
          AdvanceChars(Tokenizer, 1);
        }

        if (Tokenizer->stream[0] == '.') {
          AdvanceChars(Tokenizer, 1);

          f32 Mult = 0.1f;

          while (IsNumber(Tokenizer->stream[0])) {
            f32 Digit = (f32)(Tokenizer->stream[0] - '0');
            Value = Value + Mult * Digit;
            Mult *= 0.1f;
            AdvanceChars(Tokenizer, 1);
          }
        }

        Token.number = Value;

        if (Tokenizer->stream[0] == 'f') {
          AdvanceChars(Tokenizer, 1);
        }
      }
    }
  }

  if (Token.token_type == TokenType::kString) {
    // Do not keep "" or ''
    Token.text = std::string(Start + 1, Tokenizer->stream - 1);
  } else {
    Token.text = std::string(Start, Tokenizer->stream);
  }

  return Token;
}

beard::array<Token> Tokenize(const char* buffer) {
  beard::array<Token> tokens;

  Tokenizer tokenizer = {
      // .Filename = Filename,
      .column = 0,
      .line = 0,
      .stream = buffer,
  };

  for (;;) {
    Token token = GetToken(&tokenizer);

    tokens.add(token);

    if (token.token_type == TokenType::kEOF) {
      break;
    }
  }

  return tokens;
}