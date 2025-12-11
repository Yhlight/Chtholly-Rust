#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    Fn, Let, Mut,
    // Identifiers and literals
    Identifier, Integer,
    // Operators
    Plus, Assign, Semicolon,
    // Delimiters
    LParen, RParen, LBrace, RBrace, Colon,
    // Other
    Unknown, Eof
};

struct Token {
    TokenType type;
    std::string value;
};

#endif //CHTHOLLY_TOKEN_H
