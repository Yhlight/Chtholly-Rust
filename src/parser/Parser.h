#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Token.h"
#include "Expr.h"
#include "Stmt.h"
#include <vector>
#include <memory>

namespace Chtholly
{
    class Parser
    {
    public:
        explicit Parser(std::vector<Token> tokens);

        std::vector<std::unique_ptr<Stmt>> Parse();

    private:
        std::unique_ptr<Stmt> Declaration();
        std::unique_ptr<Stmt> LetDeclaration();
        std::unique_ptr<Stmt> Statement();
        std::unique_ptr<Stmt> ExpressionStatement();
        std::unique_ptr<Expr> Expression();
        std::unique_ptr<Expr> Primary();

        bool Match(const std::vector<TokenType>& types);
        bool Check(TokenType type);
        Token Advance();
        bool IsAtEnd();
        Token Peek();
        Token Previous();
        Token Consume(TokenType type, const std::string& message);


        std::vector<Token> m_tokens;
        int m_current = 0;
    };
}

#endif //CHTHOLLY_PARSER_H
