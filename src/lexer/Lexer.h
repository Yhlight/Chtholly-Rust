#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace Chtholly
{
    class Lexer
    {
    public:
        explicit Lexer(std::string source);

        std::vector<Token> Tokenize();

    private:
        std::string m_source;
        std::vector<Token> m_tokens;
        int m_current = 0;
    };
}

#endif //CHTHOLLY_LEXER_H
