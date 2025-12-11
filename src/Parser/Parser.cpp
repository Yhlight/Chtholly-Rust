#include "Parser.h"
#include "../AST/StmtAST.h"
#include "../AST/ExprAST.h"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    currentToken = this->lexer.nextToken();
}

void Parser::eat(TokenType type) {
    if (currentToken.type == type) {
        currentToken = lexer.nextToken();
    } else {
        throw std::runtime_error("Unexpected token");
    }
}

std::unique_ptr<ProgramAST> Parser::parse() {
    std::vector<std::unique_ptr<StmtAST>> statements;
    while (currentToken.type != TokenType::Eof) {
        statements.push_back(parseStatement());
    }
    return std::make_unique<ProgramAST>(std::move(statements));
}

std::unique_ptr<StmtAST> Parser::parseStatement() {
    switch (currentToken.type) {
        case TokenType::Let:
            return parseLetStatement();
        default:
            throw std::runtime_error("Unknown statement");
    }
}

std::unique_ptr<StmtAST> Parser::parseLetStatement() {
    eat(TokenType::Let);
    std::string variableName = currentToken.value;
    eat(TokenType::Identifier);
    eat(TokenType::Assign);
    int value = std::stoi(currentToken.value);
    eat(TokenType::Integer);
    eat(TokenType::Semicolon);

    auto numberExpr = std::make_unique<NumberExprAST>(value);
    return std::make_unique<LetStmtAST>(variableName, std::move(numberExpr));
}
