#pragma once

#include <string>
#include <variant>

enum class TokenType {
    // Keywords
    FN,
    LET,
    MUT,
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    CLASS,
    STRUCT,
    ENUM,
    IMPORT,

    // Identifiers and literals
    IDENTIFIER,
    INTEGER,
    FLOAT,
    STRING,
    CHAR,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    EQUAL,
    EQUAL_EQUAL,
    BANG,
    BANG_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    AND,
    AND_AND,
    OR,
    OR_OR,

    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    DOT,
    COLON,
    SEMICOLON,

    // End of file
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, int, double, std::string> literal;
    int line;
};
