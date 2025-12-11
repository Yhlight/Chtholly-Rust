#include "Parser.h"

namespace Chtholly {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<TranslationUnitAST> Parser::parse() {
    auto translationUnit = std::make_unique<TranslationUnitAST>();
    while (peek().type != TokenType::Eof) {
        translationUnit->addFunction(parseFunction());
    }
    return translationUnit;
}

std::unique_ptr<FunctionDeclAST> Parser::parseFunction() {
    if (match(TokenType::Fn)) {
        if (peek().type == TokenType::Identifier || peek().type == TokenType::Main) {
            const Token& nameToken = advance();
            if (match(TokenType::LParen)) {
                // For now, skip parameters
                while (peek().type != TokenType::RParen && peek().type != TokenType::Eof) {
                    advance();
                }
                if (match(TokenType::RParen)) {
                    if (match(TokenType::LBrace)) {
                        // For now, skip function body
                        while (peek().type != TokenType::RBrace && peek().type != TokenType::Eof) {
                            advance();
                        }
                        match(TokenType::RBrace);
                        return std::make_unique<FunctionDeclAST>(nameToken.value);
                    }
                }
            }
        }
    }
    // Basic error handling - just advance to the next token
    advance();
    return nullptr;
}

const Token& Parser::peek() {
    return tokens[current];
}

const Token& Parser::advance() {
    if (current < tokens.size()) {
        current++;
    }
    return tokens[current - 1];
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

} // namespace Chtholly
