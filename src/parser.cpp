#include "parser.h"
#include <stdexcept>
#include <variant>

namespace Chtholly
{
    Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

    std::vector<std::shared_ptr<Stmt>> Parser::parse()
    {
        std::vector<std::shared_ptr<Stmt>> statements;
        while (!isAtEnd())
        {
            statements.push_back(declaration());
        }
        return statements;
    }

    std::shared_ptr<Stmt> Parser::declaration()
    {
        try
        {
            if (match({TokenType::LET})) return letDeclaration();
            return statement();
        }
        catch (const std::runtime_error& error)
        {
            synchronize();
            return nullptr;
        }
    }

    std::shared_ptr<Stmt> Parser::letDeclaration()
    {
        bool isMutable = match({TokenType::MUT});
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

        std::shared_ptr<Expr> initializer = nullptr;
        if (match({TokenType::EQUAL}))
        {
            initializer = expression();
        }

        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        return std::make_shared<LetStmt>(name, initializer, isMutable);
    }

    std::shared_ptr<Stmt> Parser::statement()
    {
        return expressionStatement();
    }

    std::shared_ptr<Stmt> Parser::expressionStatement()
    {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return std::make_shared<ExpressionStmt>(expr);
    }

    std::shared_ptr<Expr> Parser::expression()
    {
        return assignment();
    }

    std::shared_ptr<Expr> Parser::assignment()
    {
        std::shared_ptr<Expr> expr = equality();

        if (match({TokenType::EQUAL}))
        {
            Token equals = previous();
            std::shared_ptr<Expr> value = assignment();

            if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr))
            {
                Token name = varExpr->name;
                return std::make_shared<AssignExpr>(name, value);
            }
            throw std::runtime_error("Invalid assignment target.");
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::equality()
    {
        std::shared_ptr<Expr> expr = comparison();

        while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = comparison();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::comparison()
    {
        std::shared_ptr<Expr> expr = term();

        while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = term();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::term()
    {
        std::shared_ptr<Expr> expr = factor();

        while (match({TokenType::MINUS, TokenType::PLUS}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = factor();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::factor()
    {
        std::shared_ptr<Expr> expr = unary();

        while (match({TokenType::SLASH, TokenType::STAR}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = unary();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::unary()
    {
        if (match({TokenType::BANG, TokenType::MINUS}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = unary();
            return std::make_shared<UnaryExpr>(op, right);
        }

        return primary();
    }

    std::shared_ptr<Expr> Parser::primary()
    {
        if (match({TokenType::FALSE})) return std::make_shared<LiteralExpr>(false);
        if (match({TokenType::TRUE})) return std::make_shared<LiteralExpr>(true);

        if (match({TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING, TokenType::CHAR}))
        {
            return std::make_shared<LiteralExpr>(std::visit(
                [](auto&& arg) -> std::variant<std::monostate, int, double, std::string, char, bool> { return arg; },
                previous().literal
            ));
        }

        if (match({TokenType::IDENTIFIER}))
        {
            return std::make_shared<VariableExpr>(previous());
        }

        if (match({TokenType::LEFT_PAREN}))
        {
            std::shared_ptr<Expr> expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return expr;
        }

        throw std::runtime_error("Expect expression.");
    }

    bool Parser::match(const std::vector<TokenType>& types)
    {
        for (TokenType type : types)
        {
            if (check(type))
            {
                advance();
                return true;
            }
        }
        return false;
    }

    Token Parser::consume(TokenType type, const std::string& message)
    {
        if (check(type)) return advance();
        throw std::runtime_error(message);
    }

    bool Parser::check(TokenType type) const
    {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token Parser::advance()
    {
        if (!isAtEnd()) current++;
        return previous();
    }

    bool Parser::isAtEnd() const
    {
        return peek().type == TokenType::END_OF_FILE;
    }

    Token Parser::peek() const
    {
        return tokens[current];
    }

    Token Parser::previous() const
    {
        return tokens[current - 1];
    }

    void Parser::synchronize()
    {
        advance();
        while (!isAtEnd())
        {
            if (previous().type == TokenType::SEMICOLON) return;
            switch (peek().type)
            {
            case TokenType::CLASS:
            case TokenType::FN:
            case TokenType::LET:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            }
            advance();
        }
    }

} // namespace Chtholly
