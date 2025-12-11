#include "Parser.h"
#include <stdexcept>

namespace Chtholly
{

Parser::Parser(const std::vector<Token>& tokens)
    : m_tokens(tokens)
{
}

std::unique_ptr<Expr> Parser::parse()
{
    try
    {
        return expression();
    }
    catch (const ParseError& error)
    {
        // For now, we just report the error and return null.
        // A more sophisticated compiler might have a dedicated error reporting system.
        fprintf(stderr, "%s\n", error.what());
        return nullptr;
    }
}

// expression -> equality
std::unique_ptr<Expr> Parser::expression()
{
    return equality();
}

// equality -> comparison ( ( "!=" | "==" ) comparison )*
std::unique_ptr<Expr> Parser::equality()
{
    auto expr = comparison();

    while (match({ TokenType::BangEqual, TokenType::EqualEqual }))
    {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
std::unique_ptr<Expr> Parser::comparison()
{
    auto expr = term();

    while (match({ TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual }))
    {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// term -> factor ( ( "-" | "+" ) factor )*
std::unique_ptr<Expr> Parser::term()
{
    auto expr = factor();

    while (match({ TokenType::Minus, TokenType::Plus }))
    {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// factor -> unary ( ( "/" | "*" ) unary )*
std::unique_ptr<Expr> Parser::factor()
{
    auto expr = unary();

    while (match({ TokenType::Slash, TokenType::Star }))
    {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// unary -> ( "!" | "-" ) unary | primary
std::unique_ptr<Expr> Parser::unary()
{
    if (match({ TokenType::Bang, TokenType::Minus }))
    {
        Token op = previous();
        auto right = unary();
        return std::make_unique<Unary>(op, std::move(right));
    }

    return primary();
}

// primary -> NUMBER | STRING | "true" | "false" | "(" expression ")"
std::unique_ptr<Expr> Parser::primary()
{
    if (match({ TokenType::False })) return std::make_unique<Literal>(false);
    if (match({ TokenType::True })) return std::make_unique<Literal>(true);

    if (match({ TokenType::Number, TokenType::String }))
    {
        // The lexer gives us the literal value as a string. We'll convert it to the right type here.
        // For simplicity, we'll assume numbers are doubles.
        if (previous().type == TokenType::Number)
        {
            return std::make_unique<Literal>(std::stod(std::string(previous().lexeme)));
        }
        return std::make_unique<Literal>(std::string(previous().lexeme));
    }

    if (match({TokenType::LeftParen})) {
        auto expr = expression();
        consume(TokenType::RightParen, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}


// Helper functions
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
    throw error(peek(), message);
}

bool Parser::check(TokenType type) const
{
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance()
{
    if (!isAtEnd()) m_current++;
    return previous();
}

bool Parser::isAtEnd() const
{
    return peek().type == TokenType::Eof;
}

Token Parser::peek() const
{
    return m_tokens[m_current];
}

Token Parser::previous() const
{
    return m_tokens[m_current - 1];
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    std::string error_message = "[line " + std::to_string(token.line) + "] Error";

    if (token.type == TokenType::Eof) {
        error_message += " at end";
    } else {
        error_message += " at '" + std::string(token.lexeme) + "'";
    }

    error_message += ": " + message;
    return ParseError(error_message);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;

        switch (peek().type) {
            case TokenType::Class:
            case TokenType::Fn:
            case TokenType::Let:
            case TokenType::For:
            case TokenType::If:
            case TokenType::While:
            case TokenType::Return:
                return;
        }

        advance();
    }
}


} // namespace Chtholly
