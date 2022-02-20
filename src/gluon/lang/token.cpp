#include "gluon/lang/token.h"

namespace gluon::lang {

const char* to_string(TokenType token_type) {
  // clang-format off
  switch (token_type)
  {
    case TokenType::Unknown: return "Unknown";
    case TokenType::OpenBrace: return "OpenBrace";
    case TokenType::CloseBrace: return "CloseBracee";
    case TokenType::OpenParen: return "OpenParen";
    case TokenType::CloseParen: return "CloseParen";
    case TokenType::OpenBracket: return "OpenBracket";
    case TokenType::CloseBracket: return "CloseBracket";
    case TokenType::Comma: return "Comma";
    case TokenType::Colon: return "Colon";
    case TokenType::Semicolon: return "Semicolon";
    case TokenType::Dot: return "Dot";
    case TokenType::Equal: return "Equal";
    case TokenType::EqualEqual: return "EqualEqual";
    case TokenType::BangEqual: return "BangEqual";
    case TokenType::Less: return "Less";
    case TokenType::LessEqual: return "LessEqual";
    case TokenType::Greater: return "Greater";
    case TokenType::GreaterEqual: return "GreaterEqual";
    case TokenType::Plus: return "Plus";
    case TokenType::PlusEqual: return "PlusEqual";
    case TokenType::Minus: return "Minus";
    case TokenType::MinusEqual: return "MinusEqual";
    case TokenType::Star: return "Star";
    case TokenType::StarEqual: return "StarEqual";
    case TokenType::Slash: return "Slash";
    case TokenType::SlashEqual: return "SlashEqual";
    case TokenType::Percent: return "Percent";
    case TokenType::PercentEqual: return "PercentEqual";
    case TokenType::Number: return "Number";
    case TokenType::String: return "String";
    case TokenType::Identifier: return "Identifier";
    case TokenType::And: return "And";
    case TokenType::Class: return "Class";
    case TokenType::Else: return "Else";
    case TokenType::False: return "False";
    case TokenType::For: return "For";
    case TokenType::Fn: return "Fn";
    case TokenType::If: return "If";
    case TokenType::Let: return "Let";
    case TokenType::Nil: return "Nil";
    case TokenType::Not: return "Not";
    case TokenType::Or: return "Or";
    case TokenType::Return: return "Return";
    case TokenType::Super: return "Super";
    case TokenType::Switch: return "Switch";
    case TokenType::This: return "This";
    case TokenType::True: return "True";
    case TokenType::While: return "While";
    case TokenType::Xor: return "Xor";
    case TokenType::Print: return "Print";
    case TokenType::EOL: return "EOL";
  }
  // clang-format on

  ASSERT_UNREACHABLE();
  return "";
}

}  // namespace gluon::lang