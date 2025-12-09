#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace Chtholly
{

    class Lexer
    {
    public:
        Lexer(const std::string& source);
        std::vector<Token> scanTokens();

    private:
        void scanToken();
        char advance();
        bool match(char expected);
        char peek() const;
        char peekNext() const;
        void addToken(TokenType type);
        void addToken(TokenType type, const std::variant<std::monostate, int, double, std::string, char>& literal);
        void stringLiteral();
        void numberLiteral();
        void identifier();

        const std::string source;
        std::vector<Token> tokens;
        int start = 0;
        int current = 0;
        int line = 1;

        static const std::unordered_map<std::string, TokenType> keywords;
    };

} // namespace Chtholly

#endif // CHTHOLLY_LEXER_H
