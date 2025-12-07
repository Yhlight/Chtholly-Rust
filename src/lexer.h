#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "token.h"
#include <string>

class Lexer {
public:
    explicit Lexer(const std::string& code);
    Token next_token();

private:
    void skip_whitespace();
    char peek();
    char advance();
    Token make_token(TokenType type, const std::string& literal);

    std::string code_;
    size_t pos_;
};

#endif //CHTHOLLY_LEXER_H
