#include "Parser.h"
#include <iostream>

namespace chtholly {

Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    // Prime the parser with the first token.
    getNextToken();
}

Token Parser::getNextToken() {
    return m_currentToken = m_lexer.getNextToken();
}

std::unique_ptr<ExprAST> Parser::logError(const char* str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::logErrorP(const char* str) {
    logError(str);
    return nullptr;
}

std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
    auto result = std::make_unique<NumberExprAST>(m_lexer.getNumber());
    getNextToken(); // consume the number
    return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseParenExpr() {
    getNextToken(); // eat (.
    auto v = parseExpression();
    if (!v) {
        return nullptr;
    }

    if (m_currentToken != Token::RParen) {
        return logError("expected ')'");
    }
    getNextToken(); // eat ).
    return v;
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr() {
    std::string idName = m_lexer.getIdentifier();
    getNextToken(); // eat identifier.

    // Simple variable ref.
    return std::make_unique<VariableExprAST>(idName);
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
    switch (m_currentToken) {
    default:
        return logError("unknown token when expecting an expression");
    case Token::Identifier:
        return parseIdentifierExpr();
    case Token::Number:
        return parseNumberExpr();
    case Token::LParen:
        return parseParenExpr();
    }
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
    auto lhs = parsePrimary();
    if (!lhs) {
        return nullptr;
    }
    // For now, we don't handle binary operators, so we just return the primary expression.
    return lhs;
}

std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
    if (m_currentToken != Token::Identifier) {
        return logErrorP("Expected function name in prototype");
    }

    std::string fnName = m_lexer.getIdentifier();
    getNextToken();

    if (m_currentToken != Token::LParen) {
        return logErrorP("Expected '(' in prototype");
    }
    getNextToken(); // eat '('.

    std::vector<std::string> argNames;
    if (m_currentToken != Token::RParen) { // if there are any arguments
        // Parse the first argument
        if (m_currentToken != Token::Identifier) {
            return logErrorP("Expected identifier in prototype");
        }
        argNames.push_back(m_lexer.getIdentifier());
        getNextToken();

        // Parse the rest of the arguments
        while (m_currentToken == Token::Comma) {
            getNextToken(); // eat ','
            if (m_currentToken != Token::Identifier) {
                return logErrorP("Expected identifier after ',' in prototype");
            }
            argNames.push_back(m_lexer.getIdentifier());
            getNextToken();
        }
    }

    if (m_currentToken != Token::RParen) {
        return logErrorP("Expected ')' in prototype");
    }

    getNextToken(); // eat ')'.

    return std::make_unique<PrototypeAST>(fnName, std::move(argNames));
}

std::unique_ptr<FunctionAST> Parser::parseDefinition() {
    getNextToken(); // eat fn.
    auto proto = parsePrototype();
    if (!proto) {
        return nullptr;
    }

    if (m_currentToken != Token::LBrace) {
         logErrorP("Expected '{' in function body");
         return nullptr;
    }
    getNextToken(); // eat {

    auto body = parseExpression();

    if (m_currentToken != Token::Semicolon) {
        logErrorP("Expected ';' after expression in function body");
        return nullptr;
    }
    getNextToken(); // eat ;

    if (m_currentToken != Token::RBrace) {
        logErrorP("Expected '}' in function body");
        return nullptr;
    }
    getNextToken(); // eat }

    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
}

std::unique_ptr<FunctionAST> Parser::handleDefinition() {
    if (auto f = parseDefinition()) {
        return f;
    }
    // Skip token for error recovery.
    getNextToken();
    return nullptr;
}

std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr() {
    auto proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(proto), parseExpression());
}

std::unique_ptr<FunctionAST> Parser::handleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (auto f = parseTopLevelExpr()) {
        return f;
    }
    // Skip token for error recovery.
    getNextToken();
    return nullptr;
}

} // namespace chtholly
