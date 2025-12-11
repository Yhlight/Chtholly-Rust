#include "Parser.h"
#include <stdexcept>
#include "Token.h"
#include "Expr.h"
#include "Stmt.h"
#include <vector>
#include <memory>

namespace Chtholly
{
    Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}

    std::vector<std::unique_ptr<Stmt>> Parser::Parse()
    {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!IsAtEnd())
        {
            statements.push_back(Declaration());
        }
        return statements;
    }

    std::unique_ptr<Stmt> Parser::Declaration()
    {
        if (Match({TokenType::Let}))
        {
            return LetDeclaration();
        }
        return Statement();
    }

    std::unique_ptr<Stmt> Parser::LetDeclaration()
    {
        bool is_mutable = Match({TokenType::Mut});
        Token name = Consume(TokenType::Identifier, "Expect variable name.");
        std::unique_ptr<Expr> initializer = nullptr;
        if (Match({TokenType::Equal}))
        {
            initializer = Expression();
        }
        else
        {
            throw std::runtime_error("Expect initializer in let declaration.");
        }
        Consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
        return std::make_unique<LetDeclStmt>(name, std::move(initializer), is_mutable);
    }

    std::unique_ptr<Stmt> Parser::Statement()
    {
        return ExpressionStatement();
    }

    std::unique_ptr<Stmt> Parser::ExpressionStatement()
    {
        std::unique_ptr<Expr> expr = Expression();
        Consume(TokenType::Semicolon, "Expect ';' after expression.");
        return std::make_unique<ExpressionStmt>(std::move(expr));
    }

    std::unique_ptr<Expr> Parser::Expression()
    {
        return Term();
    }

    std::unique_ptr<Expr> Parser::Term()
    {
        std::unique_ptr<Expr> expr = Factor();
        while (Match({TokenType::Minus, TokenType::Plus}))
        {
            Token op = Previous();
            std::unique_ptr<Expr> right = Factor();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::Factor()
    {
        std::unique_ptr<Expr> expr = Unary();
        while (Match({TokenType::Slash, TokenType::Asterisk}))
        {
            Token op = Previous();
            std::unique_ptr<Expr> right = Unary();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::Unary()
    {
        if (Match({TokenType::Bang, TokenType::Minus}))
        {
            Token op = Previous();
            std::unique_ptr<Expr> right = Unary();
            return std::make_unique<UnaryExpr>(op, std::move(right));
        }
        return Primary();
    }

    std::unique_ptr<Expr> Parser::Primary()
    {
        if (Match({TokenType::Integer, TokenType::Float, TokenType::String, TokenType::Char}))
        {
            return std::make_unique<LiteralExpr>(Previous());
        }

        if (Match({TokenType::LParen}))
        {
            std::unique_ptr<Expr> expr = Expression();
            Consume(TokenType::RParen, "Expect ')' after expression.");
            return std::make_unique<GroupingExpr>(std::move(expr));
        }

        throw std::runtime_error("Expect expression.");
    }

    bool Parser::Match(const std::vector<TokenType>& types)
    {
        for (TokenType type : types)
        {
            if (Check(type))
            {
                Advance();
                return true;
            }
        }
        return false;
    }

    bool Parser::Check(TokenType type)
    {
        if (IsAtEnd()) return false;
        return Peek().GetType() == type;
    }

    Token Parser::Advance()
    {
        if (!IsAtEnd()) m_current++;
        return Previous();
    }

    bool Parser::IsAtEnd()
    {
        return Peek().GetType() == TokenType::Eof;
    }

    Token Parser::Peek()
    {
        return m_tokens[m_current];
    }

    Token Parser::Previous()
    {
        return m_tokens[m_current - 1];
    }

    Token Parser::Consume(TokenType type, const std::string& message)
    {
        if (Check(type)) return Advance();
        throw std::runtime_error(message);
    }
}
