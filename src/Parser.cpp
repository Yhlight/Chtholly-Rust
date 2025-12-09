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
    if (peek().type == TokenType::FN) {
        return parse_function_definition();
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

std::unique_ptr<StmtAST> Parser::parse_function_definition() {
    advance(); // consume 'fn'
    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected function name after 'fn'");
    }
    std::string func_name = advance().value;

    if (peek().type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' after function name");
    }
    advance(); // consume '('

    std::vector<FunctionArg> args;
    while (peek().type != TokenType::RIGHT_PAREN) {
        if (peek().type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected argument name");
        }
        std::string arg_name = advance().value;

        if (peek().type != TokenType::COLON) {
            throw std::runtime_error("Expected ':' after argument name");
        }
        advance(); // consume ':'

        args.push_back({arg_name, parse_type()});

        if (peek().type == TokenType::COMMA) {
            advance(); // consume ','
        } else if (peek().type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expected ',' or ')' after argument");
        }
    }
    advance(); // consume ')'

    if (peek().type != TokenType::COLON) {
        throw std::runtime_error("Expected ':' for return type");
    }
    advance(); // consume ':'
    auto return_type = parse_type();

    auto body = parse_block();

    return std::make_unique<FunctionDeclAST>(func_name, std::move(args), std::move(return_type), std::move(body));
}

std::unique_ptr<BlockStmtAST> Parser::parse_block() {
    if (peek().type != TokenType::LEFT_BRACE) {
        throw std::runtime_error("Expected '{' to start a block");
    }
    advance(); // consume '{'

    auto block = std::make_unique<BlockStmtAST>();
    while (peek().type != TokenType::RIGHT_BRACE && !is_at_end()) {
        block->statements.push_back(parse_statement());
    }

    if (peek().type != TokenType::RIGHT_BRACE) {
        throw std::runtime_error("Expected '}' to end a block");
    }
    advance(); // consume '}'
    return block;
}

std::unique_ptr<TypeNameAST> Parser::parse_type() {
    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected a type name");
    }
    // This will need to be extended to handle complex types like arrays, etc.
    return std::make_unique<TypeNameAST>(advance().value);
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
    if (peek().type == TokenType::STRING_LITERAL) {
        return std::make_unique<StringExprAST>(advance().value);
    }
    if (peek().type == TokenType::IDENTIFIER) {
        return std::make_unique<VariableExprAST>(advance().value);
    }
    if (peek().type == TokenType::LEFT_PAREN) {
        advance(); // consume '('
        auto expr = parse_expression();
        if (peek().type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expected ')' after expression");
        }
        advance(); // consume ')'
        return expr;
    }
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
