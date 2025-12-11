#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source;
    int current = 0;
    int line = 1;
    int column = 1;

    char advance();
    char peek();
    char peekNext();
    bool isAtEnd();
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string& value);
    Token identifier();
    Token number();
    Token stringLiteral();
    Token charLiteral();
};

#endif //CHTHOLLY_LEXER_H
