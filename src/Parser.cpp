#include "Parser.h"
#include <stdexcept>
#include <map>

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

std::vector<std::unique_ptr<FunctionAST>> Parser::parse() {
    std::vector<std::unique_ptr<FunctionAST>> functions;
    while (currentToken.type != TokenType::EndOfFile) {
        functions.push_back(parseFunctionDefinition());
    }
    return functions;
}

std::unique_ptr<StmtAST> Parser::parseStatement() {
    switch (currentToken.type) {
        case TokenType::Let:
            return parseVarDeclStatement();
        default:
            throw std::runtime_error("Unexpected token in statement: " + currentToken.value);
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

    std::string type = "";
    if (currentToken.type == TokenType::Colon) {
        consume(TokenType::Colon);
        type = parseType();
    }

    consume(TokenType::Equal);

    auto init = parseExpression();

    consume(TokenType::Semicolon);

    return std::make_unique<VarDeclStmtAST>(name, type, isMutable, std::move(init));
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
    switch (currentToken.type) {
        case TokenType::Identifier:
            return parseIdentifierExpression();
        case TokenType::Integer:
        case TokenType::Float:
            return parseNumberExpression();
        case TokenType::String:
            return parseStringExpression();
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

std::unique_ptr<ExprAST> Parser::parseStringExpression() {
    std::string value = currentToken.value;
    consume(TokenType::String);
    return std::make_unique<StringExprAST>(value);
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
    std::vector<std::pair<std::string, std::string>> ArgNames;
    while (currentToken.type == TokenType::Identifier) {
        std::string ArgName = currentToken.value;
        consume(TokenType::Identifier);
        consume(TokenType::Colon);
        std::string ArgType = parseType();
        ArgNames.push_back({ArgName, ArgType});
        if (currentToken.type == TokenType::RightParen)
            break;
        consume(TokenType::Comma);
    }
    consume(TokenType::RightParen);

    consume(TokenType::Colon);
    std::string ReturnType = parseType();

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames), ReturnType);
}

std::unique_ptr<FunctionAST> Parser::parseFunctionDefinition() {
    consume(TokenType::Fn);
    auto Proto = parsePrototype();
    consume(TokenType::LeftBrace);

    std::vector<std::unique_ptr<StmtAST>> Body;
    while (currentToken.type != TokenType::RightBrace) {
        Body.push_back(parseStatement());
    }

    consume(TokenType::RightBrace);
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(Body));
}

std::string Parser::parseType() {
    std::string TypeName = currentToken.value;
    consume(TokenType::Identifier);
    return TypeName;
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
