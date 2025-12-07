#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    FN,
    LET,
    MUT,

    // Identifiers and literals
    IDENTIFIER,
    INTEGER,

    // Operators
    ASSIGN,
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,

    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
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
