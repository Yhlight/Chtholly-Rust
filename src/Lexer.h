#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>

namespace Chtholly {

class Lexer {
public:
    Lexer(const std::string& source);

    Token nextToken();

private:
    void skipWhitespace();
    Token identifier();
    Token number();
    Token stringLiteral();
    Token charLiteral();
    Token makeToken(TokenType type, const std::string& value);
    char peek();
    char advance();

    std::string source;
    int current;
    int line;
    int column;
    int start_line;
    int start_column;
};

} // namespace Chtholly

#endif // CHTHOLLY_LEXER_H
