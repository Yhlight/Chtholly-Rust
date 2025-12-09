#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<BlockStmtAST> Parser::parse() {
    auto block = std::make_unique<BlockStmtAST>();
    while (!is_at_end()) {
        block->statements.push_back(parse_statement());
    }
    return block;
}

Token& Parser::peek() {
    return tokens[current_token_idx];
}

Token& Parser::advance() {
    if (!is_at_end()) {
        current_token_idx++;
    }
    return tokens[current_token_idx - 1];
}

bool Parser::is_at_end() {
    return peek().type == TokenType::END_OF_FILE;
}

std::unique_ptr<StmtAST> Parser::parse_statement() {
    if (peek().type == TokenType::LET) {
        return parse_variable_declaration();
    }
    // Handle other statement types in the future
    // For now, just advance past unknown tokens to avoid infinite loops
    advance();
    return nullptr;
}

std::unique_ptr<StmtAST> Parser::parse_variable_declaration() {
    advance(); // consume 'let'
    bool is_mutable = false;
    if (peek().type == TokenType::MUT) {
        is_mutable = true;
        advance(); // consume 'mut'
    }

    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after 'let' or 'let mut'");
    }
    std::string var_name = advance().value;

    std::unique_ptr<ExprAST> initializer = nullptr;
    if (peek().type == TokenType::EQUAL) {
        advance(); // consume '='
        initializer = parse_expression();
    }

    if (peek().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after variable declaration");
    }
    advance(); // consume ';'

    return std::make_unique<VarDeclStmtAST>(var_name, is_mutable, std::move(initializer));
}

std::unique_ptr<ExprAST> Parser::parse_expression() {
    auto lhs = parse_primary();
    if (!lhs) {
        return nullptr;
    }
    return parse_binary_expression(0, std::move(lhs));
}

std::unique_ptr<ExprAST> Parser::parse_primary() {
    if (peek().type == TokenType::INTEGER_LITERAL || peek().type == TokenType::FLOAT_LITERAL) {
        return std::make_unique<NumberExprAST>(std::stod(advance().value));
    }
    if (peek().type == TokenType::IDENTIFIER) {
        return std::make_unique<VariableExprAST>(advance().value);
    }
    // Handle other primary expressions like parentheses in the future
    return nullptr;
}

int Parser::get_token_precedence() {
    switch (peek().type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 10;
        case TokenType::STAR:
        case TokenType::SLASH:
            return 20;
        default:
            return -1;
    }
}

std::unique_ptr<ExprAST> Parser::parse_binary_expression(int expr_prec, std::unique_ptr<ExprAST> lhs) {
    while (true) {
        int tok_prec = get_token_precedence();
        if (tok_prec < expr_prec) {
            return lhs;
        }

        TokenType op = advance().type;
        auto rhs = parse_primary();
        if (!rhs) {
            return nullptr;
        }

        int next_prec = get_token_precedence();
        if (tok_prec < next_prec) {
            rhs = parse_binary_expression(tok_prec + 1, std::move(rhs));
            if (!rhs) {
                return nullptr;
            }
        }
        lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
    }
}
