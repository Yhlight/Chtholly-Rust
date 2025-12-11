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

Type Parser::parseType() {
    if (currentToken.value == "i32") {
        eat(TokenType::Identifier);
        return Type(BuiltinType::I32);
    }
    if (currentToken.value == "f64") {
        eat(TokenType::Identifier);
        return Type(BuiltinType::F64);
    }
    throw std::runtime_error("Unknown type");
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
    switch (currentToken.type) {
        case TokenType::Integer: {
            int value = std::stoi(currentToken.value);
            eat(TokenType::Integer);
            return std::make_unique<NumberExprAST>(value);
        }
        case TokenType::Float: {
            double value = std::stod(currentToken.value);
            eat(TokenType::Float);
            return std::make_unique<FloatExprAST>(value);
        }
        default:
            throw std::runtime_error("Unknown expression");
    }
}

std::unique_ptr<StmtAST> Parser::parseLetStatement() {
    eat(TokenType::Let);
    std::string variableName = currentToken.value;
    eat(TokenType::Identifier);
    eat(TokenType::Colon);
    Type varType = parseType();
    eat(TokenType::Assign);
    auto value = parseExpression();
    eat(TokenType::Semicolon);

    return std::make_unique<LetStmtAST>(variableName, varType, std::move(value));
}
