#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string_view>
#include <vector>

namespace Chtholly
{

class Lexer
{
public:
    Lexer(std::string_view source);

    std::vector<Token> scanTokens();

private:
    Token scanToken();
    char advance();
    bool isAtEnd();
    bool match(char expected);
    char peek();
    char peekNext();
    void skipWhitespace();

    Token makeToken(TokenType type) const;
    Token errorToken(const char* message) const;
    Token string();
    Token number();
    Token identifier();
    TokenType identifierType();
    TokenType checkKeyword(int start, int length, const char* rest, TokenType type);


    std::string_view m_source;
    int m_start = 0;
    int m_current = 0;
    int m_line = 1;
    int m_column = 1;
};

} // namespace Chtholly

#endif // CHTHOLLY_LEXER_H
