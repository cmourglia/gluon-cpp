#include <fmt/format.h>

#include <catch2/catch_all.hpp>

#include "gluon/lang/lexer.h"

using namespace gluon::lang;

TEST_CASE("Empty string") {
  auto str = R"()";
  auto tokens = Lexer::lex(str);

  CHECK(tokens.element_count() == 0);
}

TEST_CASE("All unused") {
  auto str = R"(




    // This is a comment
        // This is a // comment with // a comment
  )";

  auto tokens = Lexer::lex(str);

  CHECK(tokens.element_count() == 0);
}

TEST_CASE("Weird string") {
  auto str = R"("This is a string with \"escaped quotes\" and an escaped \\")";
  auto tokens = Lexer::lex(str);

  CHECK(tokens.element_count() == 1);
  CHECK(tokens.get(0).type == TokenType::String);
}

TEST_CASE("Not finished string should throw") {
  auto str = R"("This is a string)";
  CHECK_THROWS(Lexer::lex(str));
}

// This test will evolve as keywords are updated
TEST_CASE("No identifiers there") {
  auto str = R"(
    and class else false for fn if let nil not or return super switch this true while xor print
  )";

  auto tokens = Lexer::lex(str);

  CHECK(tokens.element_count() == kKeywords.element_count());
  for (const auto& token : tokens) {
    CHECK(token.type != TokenType::Identifier);
  }
}

TEST_CASE("One or two char tokens") {
  auto str = R"(
    = == != < <= > >= + += - -= * *= / /= % %=
  )";

  auto tokens = Lexer::lex(str);

  CHECK(tokens.element_count() == 17);
  CHECK(tokens.get(0).type == TokenType::Equal);
  CHECK(tokens.get(1).type == TokenType::EqualEqual);
  CHECK(tokens.get(2).type == TokenType::BangEqual);
  CHECK(tokens.get(3).type == TokenType::Less);
  CHECK(tokens.get(4).type == TokenType::LessEqual);
  CHECK(tokens.get(5).type == TokenType::Greater);
  CHECK(tokens.get(6).type == TokenType::GreaterEqual);
  CHECK(tokens.get(7).type == TokenType::Plus);
  CHECK(tokens.get(8).type == TokenType::PlusEqual);
  CHECK(tokens.get(9).type == TokenType::Minus);
  CHECK(tokens.get(10).type == TokenType::MinusEqual);
  CHECK(tokens.get(11).type == TokenType::Star);
  CHECK(tokens.get(12).type == TokenType::StarEqual);
  CHECK(tokens.get(13).type == TokenType::Slash);
  CHECK(tokens.get(14).type == TokenType::SlashEqual);
  CHECK(tokens.get(15).type == TokenType::Percent);
  CHECK(tokens.get(16).type == TokenType::PercentEqual);
}

TEST_CASE("Bang is not a valid token") {
  auto str = R"(!)";

  CHECK_THROWS(Lexer::lex(str));
}

TEST_CASE("Numbers") {
  auto str = R"(42 12.0 1337.2 0123456789.987654321)";

  auto tokens = Lexer::lex(str);
  CHECK(tokens.element_count() == 4);
  CHECK(tokens.get(0).type == TokenType::Number);
  CHECK(tokens.get(1).type == TokenType::Number);
  CHECK(tokens.get(2).type == TokenType::Number);
  CHECK(tokens.get(3).type == TokenType::Number);

  auto c1 = to_number<f64>(tokens.get(0));
  auto c2 = to_number<f64>(tokens.get(1));
  auto c3 = to_number<f64>(tokens.get(2));
  auto c4 = to_number<f64>(tokens.get(3));

  CHECK(c1.has_value());
  CHECK(*c1 == 42.0);
  CHECK(c2.has_value());
  CHECK(*c2 == 12.0);
  CHECK(c3.has_value());
  CHECK(*c3 == 1337.2);
  CHECK(c4.has_value());
  CHECK(*c4 == 123456789.987654321);

  CHECK_THROWS(Lexer::lex("12."));
}

TEST_CASE("Identifiers") {
  auto str = R"(
    var toto string blblbl foo bar baz foobar _foo thefoo42 the_foo_42
  )";

  auto tokens = Lexer::lex(str);

  CHECK(tokens.element_count() == 11);
  for (const auto& token : tokens) {
    CHECK(token.type == TokenType::Identifier);
  }
}