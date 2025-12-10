#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <utility>

enum class TokenType {
    // Keywords
    FN, LET, MUT, IF, ELSE, WHILE, FOR, RETURN, CLASS, STRUCT, ENUM,
    PUBLIC, PRIVATE, REQUIRE, IMPORT, USE, AS, PACKAGE,
    SWITCH, CASE, DEFAULT, BREAK, FALLTHROUGH, DO, FOREACH,
    TRUE, FALSE,

    // Identifiers and Literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,

    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQUAL, DOUBLE_EQUAL, NOT_EQUAL,
    LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
    AND, OR, NOT,
    AMPERSAND, PIPE, CARET, TILDE,
    LEFT_SHIFT, RIGHT_SHIFT,
    PLUS_PLUS, MINUS_MINUS,
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL,

    // Punctuation
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, COLON, DOUBLE_COLON, SEMICOLON,
    ARROW, // -> for future use with return types

    // Special Tokens
    END_OF_FILE,
    UNKNOWN
};

class Token {
public:
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType type, std::string value, int line, int column)
        : type(type), value(std::move(value)), line(line), column(column) {}
};

// Forward declare the helper function
std::string tokenTypeToString(TokenType type);

#endif //CHTHOLLY_TOKEN_H
