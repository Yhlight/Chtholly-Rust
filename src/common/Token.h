#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <utility>

namespace Chtholly
{
    enum class TokenType
    {
        // Keywords
        Let,
        Mut,
        Fn,
        Return,
        If,
        Else,
        While,
        For,
        Class,
        Struct,
        Enum,

        // Identifiers and Literals
        Identifier,
        Integer,
        Float,
        String,
        Char,

        // Operators
        Plus,
        Minus,
        Asterisk,
        Slash,
        Equal,
        DoubleEqual,
        Bang,
        BangEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,

        // Delimiters
        LParen,
        RParen,
        LBrace,
        RBrace,
        LBracket,
        RBracket,
        Comma,
        Colon,
        Semicolon,

        // End of File
        Eof,

        // Unknown
        Unknown
    };

    class Token
    {
    public:
        Token(TokenType type, std::string value) : type(type), value(std::move(value)) {}

        [[nodiscard]] TokenType GetType() const { return type; }
        [[nodiscard]] const std::string& GetValue() const { return value; }

    private:
        TokenType type;
        std::string value;
    };

} // Chtholly

#endif // CHTHOLLY_TOKEN_H
