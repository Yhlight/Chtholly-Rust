#include "Parser.h"
#include "AST.h"
#include <iostream>
#include <map>
#include <stdexcept>

namespace Chtholly {

static std::map<TokenType, int> BinopPrecedence;

void InitializePrecedence() {
    BinopPrecedence[TokenType::Less] = 10;
    BinopPrecedence[TokenType::Plus] = 20;
    BinopPrecedence[TokenType::Minus] = 20;
    BinopPrecedence[TokenType::Star] = 40;
    BinopPrecedence[TokenType::Slash] = 40;
}

Parser::Parser(Lexer& lexer)
    : lexer(lexer), currentToken(lexer.nextToken()) {
    InitializePrecedence();
}

void Parser::logError(const std::string& message) {
    m_errors.push_back("Error at line " + std::to_string(currentToken.line) +
                       ", column " + std::to_string(currentToken.column) + ": " +
                       message);
}

void Parser::synchronize() {
    while (currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type == TokenType::Semicolon) {
            consume(TokenType::Semicolon);
            return;
        }
        switch (currentToken.type) {
        case TokenType::Fn:
        case TokenType::Let:
        case TokenType::Return:
            return;
        default:
            break;
        }
        currentToken = lexer.nextToken();
    }
}


std::vector<std::unique_ptr<FunctionAST>> Parser::parse() {
    std::vector<std::unique_ptr<FunctionAST>> functions;
    while (currentToken.type != TokenType::EndOfFile) {
        auto func = parseFunctionDefinition();
        if (func) {
            functions.push_back(std::move(func));
        }
    }
    return functions;
}

std::unique_ptr<StmtAST> Parser::parseStatement() {
    try {
        switch (currentToken.type) {
        case TokenType::Let:
            return parseVarDeclStatement();
        case TokenType::Return:
            return parseReturnStatement();
        default:
            return parseExpressionStatement();
        }
    } catch (const std::runtime_error& e) {
        logError(e.what());
        synchronize();
        return nullptr;
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

    Type* ty = nullptr;
    if (currentToken.type == TokenType::Colon) {
        consume(TokenType::Colon);
        ty = parseType();
    }

    consume(TokenType::Equal);

    auto init = parseExpression();

    consume(TokenType::Semicolon);

    return std::make_unique<VarDeclStmtAST>(name, ty, isMutable, std::move(init));
}

std::unique_ptr<StmtAST> Parser::parseReturnStatement() {
    consume(TokenType::Return);
    auto value = parseExpression();
    consume(TokenType::Semicolon);
    return std::make_unique<ReturnStmtAST>(std::move(value));
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
            logError("Unexpected token in expression: " + currentToken.value);
            return nullptr;
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
        int TokPrec = getTokPrecedence();

        if (TokPrec < ExprPrec)
            return LHS;

        TokenType BinOp = currentToken.type;
        consume(currentToken.type);

        auto RHS = parsePrimary();

        int NextPrec = getTokPrecedence();
        if (TokPrec < NextPrec) {
            RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS));
        }

        LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}

int Parser::getTokPrecedence() {
    if (BinopPrecedence.find(currentToken.type) == BinopPrecedence.end())
        return -1;
    return BinopPrecedence[currentToken.type];
}

std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
    std::string FnName = currentToken.value;
    consume(TokenType::Identifier);

    consume(TokenType::LeftParen);
    std::vector<std::pair<std::string, Type*>> ArgNames;
    while (currentToken.type == TokenType::Identifier) {
        std::string ArgName = currentToken.value;
        consume(TokenType::Identifier);
        consume(TokenType::Colon);
        auto ArgType = parseType();
        ArgNames.push_back({ArgName, ArgType});
        if (currentToken.type == TokenType::RightParen)
            break;
        consume(TokenType::Comma);
    }
    consume(TokenType::RightParen);

    consume(TokenType::Colon);
    auto ReturnType = parseType();

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames), ReturnType);
}

std::unique_ptr<FunctionAST> Parser::parseFunctionDefinition() {
    try {
        consume(TokenType::Fn);
        auto Proto = parsePrototype();
        if (!Proto) return nullptr;

        consume(TokenType::LeftBrace);

        std::vector<std::unique_ptr<StmtAST>> Body;
        while (currentToken.type != TokenType::RightBrace &&
               currentToken.type != TokenType::EndOfFile) {
            auto stmt = parseStatement();
            if (stmt) {
                Body.push_back(std::move(stmt));
            }
        }

        consume(TokenType::RightBrace);
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(Body));

    } catch (const std::runtime_error& e) {
        logError(e.what());
        synchronize();
        return nullptr;
    }
}

Type* Parser::parseType() {
    std::string TypeName = currentToken.value;
    consume(TokenType::Identifier);
    if (TypeName == "i32") {
        return Type::getIntegerTy();
    } else if (TypeName == "f64") {
        return Type::getFloatTy();
    } else if (TypeName == "void") {
        return Type::getVoidTy();
    }
    return nullptr;
}

void Parser::consume(TokenType expected) {
    if (currentToken.type == expected) {
        currentToken = lexer.nextToken();
    } else {
        throw std::runtime_error("Expected token " + tokenTypeToString(expected) +
                                 " but got " +
                                 tokenTypeToString(currentToken.type));
    }
}

std::unique_ptr<StmtAST> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    if (!expr) {
        return nullptr;
    }
    consume(TokenType::Semicolon);
    return std::make_unique<ExprStmtAST>(std::move(expr));
}

} // namespace Chtholly
