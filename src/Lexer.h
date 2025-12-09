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
    int current_pos = 0;
    int line = 1;
    int column = 1;

    Token nextToken();
    char peek();
    char advance();
    void skipWhitespace();
    Token identifier();
    Token number();
    Token stringLiteral();
    Token charLiteral();
};

#endif //CHTHOLLY_LEXER_H
