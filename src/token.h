#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    FN,
    LET,
    MUT,
    TRUE,
    FALSE,
    IF,
    ELSE,
    WHILE,
    FOR,

    // Identifiers and literals
    IDENTIFIER,
    INTEGER,

    // Operators
    ASSIGN,
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    BANG,
    LT,
    GT,
    EQ,
    NOT_EQ,
    INC,
    DEC,

    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    COLON,
    SEMICOLON,

    // End of file
    END_OF_FILE,

    // Illegal token
    ILLEGAL
};

struct Token {
    TokenType type;
    std::string literal;
};

#endif //CHTHOLLY_TOKEN_H
