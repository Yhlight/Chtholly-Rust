#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();

private:
    std::string source;
    size_t position;

    char currentChar();
    void advance();
    void skipWhitespace();
    Token integer();
    Token identifier();
};

#endif //CHTHOLLY_LEXER_H
