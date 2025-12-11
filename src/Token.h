#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    FN, LET, MUT, IF, ELSE, SWITCH, CASE, WHILE, FOR, DO, RETURN,
    CLASS, STRUCT, ENUM, PUBLIC, PRIVATE, CONST,

    // Identifiers and Literals
    IDENTIFIER,
    INTEGER,
    FLOAT,
    STRING,
    CHAR,

    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQUAL, EQUAL_EQUAL, BANG, BANG_EQUAL,
    GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
    AMPERSAND, AMPERSAND_AMPERSAND, PIPE, PIPE_PIPE,
    CARET, TILDE, LEFT_SHIFT, RIGHT_SHIFT,

    // Assignment
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL,

    // Increment and Decrement
    PLUS_PLUS, MINUS_MINUS,

    // Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    COMMA, DOT, COLON, SEMICOLON,

    // Special
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

#endif //CHTHOLLY_TOKEN_H
