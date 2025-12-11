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
    case '.':
        return 50; // highest
    case '*':
    case '/':
        return 40;
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

std::unique_ptr<Type> Parser::parseType() {
    Token typeToken = m_currentToken;
    getNextToken(); // consume type token
    switch (typeToken) {
    case Token::I32:
        return std::make_unique<I32Type>();
    case Token::F64:
        return std::make_unique<F64Type>();
    case Token::Bool:
        return std::make_unique<BoolType>();
    case Token::Char:
        return std::make_unique<CharType>();
    case Token::Void:
        return std::make_unique<VoidType>();
    case Token::Identifier:
        return std::make_unique<StructType>(m_lexer.getIdentifier());
    default:
        logError("unknown type when expecting a type");
        return nullptr;
    }
}

std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
    std::unique_ptr<ExprAST> result;
    if (m_currentToken == Token::IntNumber) {
        result = std::make_unique<NumberExprAST>(m_lexer.getIntNumber());
    } else {
        result = std::make_unique<NumberExprAST>(m_lexer.getFloatNumber());
    }
    getNextToken(); // consume the number
    return result;
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

    if (m_currentToken == Token::LBrace) {
        getNextToken(); // eat {.
        std::vector<std::unique_ptr<ExprAST>> args;
        if (m_currentToken != Token::RBrace) {
            while (true) {
                if (auto arg = parseExpression())
                    args.push_back(std::move(arg));
                else
                    return nullptr;

                if (m_currentToken == Token::RBrace)
                    break;

                if (m_currentToken != Token::Comma)
                    return logError("Expected '}' or ',' in argument list");
                getNextToken();
            }
        }
        getNextToken(); // eat }.
        return std::make_unique<StructInitExprAST>(idName, std::move(args));
    }

    // Simple variable ref.
    return std::make_unique<VariableExprAST>(idName);
}

std::unique_ptr<ExprAST> Parser::parseLetExpr() {
    getNextToken(); // eat let.

    bool isMutable = false;
    if (m_currentToken == Token::Mut) {
        isMutable = true;
        getNextToken(); // eat mut.
    }

    if (m_currentToken != Token::Identifier)
        return logError("expected identifier after let");

    std::string idName = m_lexer.getIdentifier();
    getNextToken(); // eat identifier.

    std::unique_ptr<Type> type;
    if (m_currentToken == Token::Colon) {
        getNextToken(); // eat :.
        type = parseType();
        if (!type) {
            return nullptr;
        }
    }

    if (m_currentToken != Token::Assign)
        return logError("expected '=' after identifier");
    getNextToken(); // eat =.

    auto init = parseExpression();
    if (!init)
        return nullptr;

    return std::make_unique<LetExprAST>(idName, isMutable, std::move(type), std::move(init));
}

std::unique_ptr<ExprAST> Parser::parseIfExpr() {
    getNextToken(); // eat if.

    if (m_currentToken != Token::LParen)
        return logError("expected '(' after if");

    auto cond = parseExpression();
    if (!cond)
        return nullptr;

    if (m_currentToken != Token::LBrace)
        return logError("expected '{' after if condition");
    getNextToken(); // eat {.

    std::vector<std::unique_ptr<ExprAST>> thenBody;
    while (m_currentToken != Token::RBrace) {
        auto expr = parseExpression();
        if (!expr)
            return nullptr;
        thenBody.push_back(std::move(expr));

        if (m_currentToken != Token::Semicolon) {
            logError("Expected ';' after expression in if body");
            return nullptr;
        }
        getNextToken(); // eat ;
    }
    getNextToken(); // eat }.

    std::vector<std::unique_ptr<ExprAST>> elseBody;
    if (m_currentToken == Token::Else) {
        getNextToken(); // eat else.

        if (m_currentToken != Token::LBrace)
            return logError("expected '{' after else");
        getNextToken(); // eat {.

        while (m_currentToken != Token::RBrace) {
            auto expr = parseExpression();
            if (!expr)
                return nullptr;
            elseBody.push_back(std::move(expr));

            if (m_currentToken != Token::Semicolon) {
                logError("Expected ';' after expression in else body");
                return nullptr;
            }
            getNextToken(); // eat ;
        }
        getNextToken(); // eat }.
    }

    return std::make_unique<IfExprAST>(std::move(cond), std::move(thenBody), std::move(elseBody));
}

std::unique_ptr<ExprAST> Parser::parseStructDef() {
    getNextToken(); // eat struct.

    if (m_currentToken != Token::Identifier)
        return logError("expected identifier after struct");
    std::string name = m_lexer.getIdentifier();
    getNextToken();

    if (m_currentToken != Token::LBrace)
        return logError("expected '{' in struct definition");
    getNextToken(); // eat {.

    std::vector<std::pair<std::string, std::unique_ptr<Type>>> members;
    while (m_currentToken != Token::RBrace) {
        if (m_currentToken != Token::Identifier)
            return logError("expected identifier in struct member");
        std::string memberName = m_lexer.getIdentifier();
        getNextToken();

        if (m_currentToken != Token::Colon)
            return logError("expected ':' in struct member");
        getNextToken(); // eat :.

        auto type = parseType();
        if (!type)
            return nullptr;
        members.push_back(std::make_pair(memberName, std::move(type)));

        if (m_currentToken != Token::Semicolon)
            return logError("expected ';' after struct member");
        getNextToken(); // eat ;.
    }
    getNextToken(); // eat }.

    return std::make_unique<StructDefAST>(name, std::move(members));
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
    switch (m_currentToken) {
    default:
        return logError("unknown token when expecting an expression");
    case Token::Identifier:
        return parseIdentifierExpr();
    case Token::IntNumber:
    case Token::FloatNumber:
        return parseNumberExpr();
    case Token::LParen:
        return parseParenExpr();
    case Token::If:
        return parseIfExpr();
    case Token::Let:
        return parseLetExpr();
    case Token::Struct:
        return parseStructDef();
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

    if (binOp == '.') {
        if (auto* vr = dynamic_cast<VariableExprAST*>(rhs.get())) {
            lhs = std::make_unique<MemberAccessExprAST>(std::move(lhs), vr->getName());
        } else {
            return logError("expected identifier after '.'");
        }
    } else {
        lhs = std::make_unique<BinaryExprAST>(binOp, std::move(lhs),
                                               std::move(rhs));
    }
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

    std::vector<std::pair<std::string, std::unique_ptr<Type>>> args;
    if (m_currentToken != Token::RParen) { // if there are any arguments
        // Parse the first argument
        if (m_currentToken != Token::Identifier) {
            return logErrorP("Expected identifier in prototype");
        }
        std::string argName = m_lexer.getIdentifier();
        getNextToken();

        if (m_currentToken != Token::Colon) {
            return logErrorP("Expected ':' in prototype");
        }
        getNextToken(); // eat :.
        auto type = parseType();
        if (!type) {
            return nullptr;
        }
        args.push_back(std::make_pair(argName, std::move(type)));

        // Parse the rest of the arguments
        while (m_currentToken == Token::Comma) {
            getNextToken(); // eat ','
            if (m_currentToken != Token::Identifier) {
                return logErrorP("Expected identifier after ',' in prototype");
            }
            argName = m_lexer.getIdentifier();
            getNextToken();

            if (m_currentToken != Token::Colon) {
                return logErrorP("Expected ':' in prototype");
            }
            getNextToken(); // eat :.
            type = parseType();
            if (!type) {
                return nullptr;
            }
            args.push_back(std::make_pair(argName, std::move(type)));
        }
    }

    if (m_currentToken != Token::RParen) {
        return logErrorP("Expected ')' in prototype");
    }
    getNextToken(); // eat ')'.

    std::unique_ptr<Type> returnType;
    if (m_currentToken == Token::Colon) {
        getNextToken(); // eat :.
        returnType = parseType();
        if (!returnType) {
            return nullptr;
        }
    } else {
        // Default to void return type if not specified
        returnType = std::make_unique<VoidType>();
    }

    return std::make_unique<PrototypeAST>(fnName, std::move(args), std::move(returnType));
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
