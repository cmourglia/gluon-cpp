#include <gluon/lang/lexer.h>

#include <beard/io/io.h>

#include <algorithm>
#include <codecvt>

namespace gluon::lang {

inline bool is_alpha(char ch) {
  bool is_lower_case = (ch >= 'a' && ch <= 'z');
  bool is_upper_case = (ch >= 'A' && ch <= 'Z');
  bool is_alpha = is_lower_case || is_upper_case;

  return is_alpha;
}

inline bool is_number(char ch) {
  bool is_number = (ch >= '0' && ch <= '9');
  return is_number;
}

inline bool is_valid_identifier_character(char ch) {
  bool is_valid = is_alpha(ch) || is_number(ch) || ch == '_';
  return is_valid;
}

beard::array<Token> Lexer::lex(std::string_view source) {
  beard::array<Token> tokens;
  Lexer lexer{source};

  while (true) {
    if (auto tk = lexer.next(); tk.has_value()) {
      tokens.add(std::move(*tk));
    } else {
      break;
    }
  }

  return tokens;
}

Lexer::Lexer(std::string_view input) : m_source{input} {}

Token Lexer::make_token(TokenType token_type) {
  Token token = {
      .lexeme = m_source.substr(m_start, m_current - m_start),
      .column = m_start_column,
      .line = m_start_line,
      .type = token_type,
  };

  return token;
}

void Lexer::skip_empty() {
  while (!done()) {
    u8 ch = peek();

    switch (ch) {
      // TODO: At some point, we will want to parse EOLs and allow
      // not requiring semicolons.
      // For now we do not handle it at all.
      case '\n': {
        m_column = 0;
        m_line += 1;
        advance();
      } break;

      case ' ':
      case '\r':
      case '\t': {
        advance();
      } break;

      case '/': {
        if (peek_next() != '/') {
          return;
        }

        while (peek() != '\n')
          advance();
      } break;

      default:
        return;
    }
  }
}

std::optional<Token> Lexer::next() {
  skip_empty();

  if (m_current == m_source.length())
    return std::nullopt;

  m_start = m_current;
  m_start_line = m_line;
  m_start_column = m_column;

  u8 ch = advance();

  switch (ch) {
    case '\0':
      return std::nullopt;
    case '{':
      return make_token(TokenType::OpenBrace);
    case '}':
      return make_token(TokenType::CloseBrace);
    case '(':
      return make_token(TokenType::OpenParen);
    case ')':
      return make_token(TokenType::CloseParen);
    case '[':
      return make_token(TokenType::OpenBracket);
    case ']':
      return make_token(TokenType::CloseBracket);
    case ',':
      return make_token(TokenType::Comma);
    case ':':
      return make_token(TokenType::Colon);
    case ';':
      return make_token(TokenType::Semicolon);
    case '.':
      return make_token(TokenType::Dot);
    case '"':
      return handle_string();
    case '=':
      return handle_one_or_two_char_token('=', TokenType::Equal,
                                          TokenType::EqualEqual);
    case '>':
      return handle_one_or_two_char_token('=', TokenType::Greater,
                                          TokenType::GreaterEqual);
    case '<':
      return handle_one_or_two_char_token('=', TokenType::Less,
                                          TokenType::LessEqual);
    case '+':
      return handle_one_or_two_char_token('=', TokenType::Plus,
                                          TokenType::PlusEqual);
    case '-':
      return handle_one_or_two_char_token('=', TokenType::Minus,
                                          TokenType::MinusEqual);
    case '*':
      return handle_one_or_two_char_token('=', TokenType::Star,
                                          TokenType::StarEqual);
    case '/':
      return handle_one_or_two_char_token('=', TokenType::Slash,
                                          TokenType::SlashEqual);
    case '%':
      return handle_one_or_two_char_token('=', TokenType::Percent,
                                          TokenType::PercentEqual);

    case '!':
      if (match('=')) {
        return make_token(TokenType::BangEqual);
      } else {
        throw std::runtime_error{
            "! is not a valid token by itself. Maybe you meant to use `not` ?"};
      }

    default:
      if (is_number(ch)) {
        return handle_number();
      } else if (is_valid_identifier_character(ch)) {
        return handle_identifier();
      } else {
        throw std::runtime_error{"Invalid character"};
      }
  }
}

Token Lexer::handle_one_or_two_char_token(char ch,
                                          TokenType one_char,
                                          TokenType two_char) {
  if (match(ch)) {
    return make_token(two_char);
  } else {
    return make_token(one_char);
  }
}

Token Lexer::handle_string() {
  // Skip first quote
  advance();

  while (true) {
    if (done()) {
      throw std::runtime_error{"Unmatched string"};
    }

    char ch = peek();
    if (ch == '\\') {
      // Skip next character
      advance();
      advance();
    } else if (ch == '"') {
      auto token = make_token(TokenType::String);
      // We do not want to include the enclosing quotes
      token.lexeme = m_source.substr(m_start + 1, (m_current - m_start) - 1);
      advance();

      return token;
    } else {
      advance();
    }
  }
}

Token Lexer::handle_number() {
  while (!done() && is_number(peek())) {
    advance();
  }

  if (match('.')) {
    if (!is_number(peek())) {
      throw std::runtime_error{"Number cannot finish by `.`"};
    }
    while (!done() && is_number(peek())) {
      advance();
    }
  }

  return make_token(TokenType::Number);
}

Token Lexer::handle_identifier() {
  while (is_valid_identifier_character(peek())) {
    advance();
  }

  auto token = make_token(TokenType::Identifier);
  if (auto it = kKeywords.find(token.lexeme); it != kKeywords.end()) {
    token.type = it->second;
  }

  return token;
}

bool Lexer::done() {
  return m_current >= m_source.size();
}

u8 Lexer::advance() {
  m_column += 1;
  m_current += 1;

  return m_source[m_current - 1];
}

u8 Lexer::peek() {
  if (done()) {
    return '\0';
  }

  return m_source[m_current];
}

u8 Lexer::peek_next() {
  if (m_current + 1 >= m_source.size()) {
    return '\0';
  }

  return m_source[m_current + 1];
}

u8 Lexer::previous() {
  return m_source[m_current - 1];
}

bool Lexer::match(u8 ch) {
  char next = peek();
  if (next == ch) {
    advance();
    return true;
  }
  return false;
}

}  // namespace gluon::lang