#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>

namespace Chtholly
{

    class Parser
    {
    public:
        Parser(std::vector<Token> tokens);
        std::vector<std::shared_ptr<Stmt>> parse();

    private:
        std::shared_ptr<Stmt> declaration();
        std::shared_ptr<Stmt> letDeclaration(bool isMutable);
        std::shared_ptr<Stmt> statement();
        std::shared_ptr<Stmt> ifStatement();
        std::shared_ptr<Stmt> whileStatement();
        std::shared_ptr<Stmt> forStatement();
        std::vector<std::shared_ptr<Stmt>> block();
        std::shared_ptr<Stmt> expressionStatement();
        std::shared_ptr<Expr> expression();
        std::shared_ptr<Expr> assignment();
        std::shared_ptr<Expr> equality();
        std::shared_ptr<Expr> comparison();
        std::shared_ptr<Expr> term();
        std::shared_ptr<Expr> factor();
        std::shared_ptr<Expr> unary();
        std::shared_ptr<Expr> primary();

        bool match(const std::vector<TokenType>& types);
        Token consume(TokenType type, const std::string& message);
        bool check(TokenType type) const;
        Token advance();
        bool isAtEnd() const;
        Token peek() const;
        Token previous() const;
        void synchronize();

        bool hadError_ = false;

        void parseError(const Token& token, const std::string& message);

        std::vector<Token> tokens;
        int current = 0;
    public:
        bool hadError() const { return hadError_; }
    };

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
