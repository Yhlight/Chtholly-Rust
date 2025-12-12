#include "Parser.h"
#include <stdexcept>
#include <map>

namespace Chtholly {

static std::map<char, int> BinopPrecedence;

void InitializePrecedence() {
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 20;
    BinopPrecedence['*'] = 40;
    BinopPrecedence['/'] = 40;
}

Parser::Parser(Lexer& lexer)
    : lexer(lexer), currentToken(lexer.nextToken()) {
    InitializePrecedence();
}

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
    auto LHS = parsePrimary();
    return parseBinOpRHS(0, std::move(LHS));
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
        case TokenType::Identifier:
            return parseIdentifierExpression();
        case TokenType::Integer:
        case TokenType::Float:
            return parseNumberExpression();
        case TokenType::LeftParen:
            consume(TokenType::LeftParen);
            {
                auto V = parseExpression();
                consume(TokenType::RightParen);
                return V;
            }
        default:
            throw std::runtime_error("Unexpected token in expression: " + currentToken.value);
    }
}

std::unique_ptr<ExprAST> Parser::parseNumberExpression() {
    double value = std::stod(currentToken.value);
    consume(currentToken.type);
    return std::make_unique<NumberExprAST>(value);
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpression() {
    std::string IdName = currentToken.value;
    consume(TokenType::Identifier);

    if (currentToken.type != TokenType::LeftParen)
        return std::make_unique<VariableExprAST>(IdName);

    consume(TokenType::LeftParen);
    std::vector<std::unique_ptr<ExprAST>> Args;
    if (currentToken.type != TokenType::RightParen) {
        while (true) {
            if (auto Arg = parseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;

            if (currentToken.type == TokenType::RightParen)
                break;

            consume(TokenType::Comma);
        }
    }

    consume(TokenType::RightParen);

    return std::make_unique<CallExprAST>(IdName, std::move(Args));
}


std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS) {
    while (true) {
        int TokPrec = GetTokPrecedence();

        if (TokPrec < ExprPrec)
            return LHS;

        char BinOp = currentToken.value[0];
        consume(currentToken.type);

        auto RHS = parsePrimary();

        int NextPrec = GetTokPrecedence();
        if (TokPrec < NextPrec) {
            RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS));
        }

        LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}

int Parser::GetTokPrecedence() {
    if (currentToken.type < TokenType::Plus || currentToken.type > TokenType::MinusMinus)
        return -1;

    int TokPrec = BinopPrecedence[currentToken.value[0]];
    if (TokPrec <= 0)
        return -1;
    return TokPrec;
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
