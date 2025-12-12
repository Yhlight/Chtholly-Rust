#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

namespace Chtholly {

enum class TokenType {
    // End of file
    EndOfFile,

    // Keywords
    Fn,
    Let,
    Mut,
    If,
    Else,
    While,
    For,
    Do,
    Switch,
    Case,
    Class,
    Struct,
    Enum,
    Public,
    Private,
    Return,
    Import,
    Use,
    As,
    Package,
    Const,

    // Identifiers and literals
    Identifier,
    Integer,
    Float,
    String,
    Char,

    // Operators
    Plus,          // +
    Minus,         // -
    Star,          // *
    Slash,         // /
    Percent,       // %
    Equal,         // =
    EqualEqual,    // ==
    Bang,          // !
    BangEqual,     // !=
    Less,          // <
    LessEqual,     // <=
    Greater,       // >
    GreaterEqual,  // >=
    Ampersand,     // &
    AmpersandAmpersand, // &&
    Pipe,          // |
    PipePipe,      // ||
    Caret,         // ^
    Tilde,         // ~
    LeftShift,     // <<
    RightShift,    // >>
    PlusPlus,      // ++
    MinusMinus,    // --

    // Delimiters
    LeftParen,     // (
    RightParen,    // )
    LeftBrace,     // {
    RightBrace,    // }
    LeftBracket,   // [
    RightBracket,  // ]
    Comma,         // ,
    Dot,           // .
    Colon,         // :
    Semicolon,     // ;

    // Unknown
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

} // namespace Chtholly

#endif // CHTHOLLY_TOKEN_H
