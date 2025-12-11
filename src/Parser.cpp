#include "Parser.h"
#include <iostream>

namespace chtholly {

// Helper function to get the precedence of the current token.
static int GetTokPrecedence(Token tok) {
    if (!isascii(static_cast<char>(tok)))
        return -1;

    switch (static_cast<char>(tok)) {
    case '<':
    case '>':
        return 10;
    case '+':
    case '-':
        return 20;
    case '*':
    case '/':
        return 40; // highest
    default:
        return -1;
    }
}

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

std::unique_ptr<ExprAST> Parser::parseLetExpr() {
    getNextToken(); // eat let.

    if (m_currentToken != Token::Identifier)
        return logError("expected identifier after let");

    std::string idName = m_lexer.getIdentifier();
    getNextToken(); // eat identifier.

    if (m_currentToken != Token::Assign)
        return logError("expected '=' after identifier");
    getNextToken(); // eat =.

    auto init = parseExpression();
    if (!init)
        return nullptr;

    return std::make_unique<LetExprAST>(idName, std::move(init));
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
    case Token::Let:
        return parseLetExpr();
    }
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int exprPrec,
                                              std::unique_ptr<ExprAST> lhs) {
  while (true) {
    int tokPrec = GetTokPrecedence(m_currentToken);
    if (tokPrec < exprPrec)
      return lhs;

    int binOp = static_cast<char>(m_currentToken);
    getNextToken();

    auto rhs = parsePrimary();
    if (!rhs)
      return nullptr;

    int nextPrec = GetTokPrecedence(m_currentToken);
    if (tokPrec < nextPrec) {
      rhs = parseBinOpRHS(tokPrec + 1, std::move(rhs));
      if (!rhs)
        return nullptr;
    }

    lhs = std::make_unique<BinaryExprAST>(binOp, std::move(lhs),
                                           std::move(rhs));
  }
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
    auto lhs = parsePrimary();
    if (!lhs)
        return nullptr;

    return parseBinOpRHS(0, std::move(lhs));
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

    std::vector<std::unique_ptr<ExprAST>> body;
    while (m_currentToken != Token::RBrace) {
        auto expr = parseExpression();
        if (!expr)
            return nullptr;
        body.push_back(std::move(expr));

        if (m_currentToken != Token::Semicolon) {
            logErrorP("Expected ';' after expression in function body");
            return nullptr;
        }
        getNextToken(); // eat ;
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
    std::vector<std::unique_ptr<ExprAST>> body;
    body.push_back(parseExpression());
    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
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
