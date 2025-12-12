#include "Parser.h"
#include <stdexcept>

namespace Chtholly {

Parser::Parser(Lexer& lexer)
    : lexer(lexer), currentToken(lexer.nextToken()) {}

std::vector<std::unique_ptr<StmtAST>> Parser::parse() {
    std::vector<std::unique_ptr<StmtAST>> statements;
    while (currentToken.type != TokenType::EndOfFile) {
        statements.push_back(parseStatement());
    }
    return statements;
}

std::unique_ptr<StmtAST> Parser::parseStatement() {
    switch (currentToken.type) {
        case TokenType::Let:
            return parseVarDeclStatement();
        default:
            throw std::runtime_error("Unexpected token: " + currentToken.value);
    }
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
    return parsePrimary();
}

std::unique_ptr<StmtAST> Parser::parseVarDeclStatement() {
    consume(TokenType::Let);

    bool isMutable = false;
    if (currentToken.type == TokenType::Mut) {
        isMutable = true;
        consume(TokenType::Mut);
    }

    std::string name = currentToken.value;
    consume(TokenType::Identifier);

    consume(TokenType::Equal);

    auto init = parseExpression();

    consume(TokenType::Semicolon);

    return std::make_unique<VarDeclStmtAST>(name, isMutable, std::move(init));
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
    switch (currentToken.type) {
        case TokenType::Integer:
        case TokenType::Float:
            return parseNumberExpression();
        default:
            throw std::runtime_error("Unexpected token in expression: " + currentToken.value);
    }
}

std::unique_ptr<ExprAST> Parser::parseNumberExpression() {
    double value = std::stod(currentToken.value);
    consume(currentToken.type);
    return std::make_unique<NumberExprAST>(value);
}

void Parser::consume(TokenType expected) {
    if (currentToken.type == expected) {
        currentToken = lexer.nextToken();
    } else {
        throw std::runtime_error("Expected token " + std::to_string((int)expected) +
                                 " but got " + std::to_string((int)currentToken.type));
    }
}

} // namespace Chtholly
