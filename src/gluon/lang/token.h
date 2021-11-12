#pragma once

#include <beard/core/macros.h>

#include <string>

struct TokenType
{
    enum Enum
    {
        kUnknown,

        kOpenBrace,
        kCloseBrace,
        kOpenParen,
        kCloseParen,
        kOpenBracket,
        kCloseBracket,

        kComma,
        kColon,
        kSemicolon,
        kDot,

        kAdd,      // +
        kSubtract, // -
        kMultiply, // *
        kDivide,   // /
        kModulo,   // %
        kPower,    // ^

        kIf,    // if
        kElse,  // else
        kWhile, // while
        kFor,   // for
        kLet,   // let

        kNull,  // null
        kTrue,  // true
        kFalse, // false

        kAnd, // and
        kOr,  // or
        kNot, // not

        kEquals,        // ==
        kNotEquals,     // !=
        kGreater,       // >
        kGreaterEquals, // >=
        kLess,          // <
        kLessEquals,    // <=

        kAssign,         // =
        kAddAssign,      // +=
        kSubtractAssign, // -=
        kMultiplyAssign, // *=
        kDivideAssign,   // /=
        kModuloAssign,   // %=
        kPowerAssign,    // ^=

        kNumber,
        kString,
        kIdentifier,

        kComment,

        // Spacing,
        kEOL,
        kEOF,
    };
};

std::string ToString(TokenType::Enum token_type);

struct Token
{
    std::string filename;
    u32         column, line;

    TokenType::Enum token_type;
    std::string     text;
    f32             number;
};
