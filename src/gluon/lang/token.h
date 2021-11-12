#pragma once

#include <beard/core/macros.h>

#include <string>

enum class ETokenType
{
    Unknown,

    OpenBrace,
    CloseBrace,
    OpenParen,
    CloseParen,
    OpenBracket,
    CloseBracket,

    Comma,
    Colon,
    Semicolon,
    Dot,

    Add,      // +
    Subtract, // -
    Multiply, // *
    Divide,   // /
    Modulo,   // %
    Power,    // ^

    If,    // if
    Else,  // else
    While, // while
    For,   // for
    Let,   // let

    Null,  // null
    True,  // true
    False, // false

    And, // and
    Or,  // or
    Not, // not

    Equals,        // ==
    NotEquals,     // !=
    Greater,       // >
    GreaterEquals, // >=
    Less,          // <
    LessEquals,    // <=

    Assign,         // =
    AddAssign,      // +=
    SubtractAssign, // -=
    MultiplyAssign, // *=
    DivideAssign,   // /=
    ModuloAssign,   // %=
    PowerAssign,    // ^=

    Number,
    String,
    Identifier,

    Comment,

    // Spacing,
    EndOfLine,
    EndOfStream,
};

std::string ToString(ETokenType TokenType);

struct ZToken
{
    std::string Filename;
    u32         Column, Line;

    ETokenType  Type;
    std::string Text;
    f32         Number;
};
