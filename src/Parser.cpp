#include "Parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Program Parser::parse() {
    Program statements;
    while (!isAtEnd()) {
        auto stmt = declaration();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::LET})) return varDeclaration();

    // For now, skip tokens until the next statement to avoid infinite loops
    // A more robust implementation would use error productions or a panic mode
    // with synchronization.
    advance();
    return nullptr;
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    bool isMutable = false;
    if (previous().type == TokenType::LET) {
        if(match({TokenType::MUT})) {
            isMutable = true;
        }
    }

    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VarDeclStmt>(name, std::move(initializer), isMutable);
}

std::unique_ptr<Expr> Parser::expression() {
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous());
    }

    throw std::runtime_error("Expect expression.");
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}
