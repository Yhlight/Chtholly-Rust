#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "AST/Expr.h"
#include "Token.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace Chtholly
{

class Parser
{
public:
    Parser(const std::vector<Token>& tokens);

    std::unique_ptr<Expr> parse();

private:
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type) const;
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;

    void synchronize();

    struct ParseError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    ParseError error(const Token& token, const std::string& message);

    const std::vector<Token>& m_tokens;
    int m_current = 0;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
