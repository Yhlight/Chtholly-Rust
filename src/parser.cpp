#include "parser.h"

Parser::Parser(Lexer& lexer) : lexer_(lexer) {
    // Read two tokens, so cur_token_ and peek_token_ are both set
    next_token();
    next_token();
}

void Parser::next_token() {
    cur_token_ = peek_token_;
    peek_token_ = lexer_.next_token();
}

std::unique_ptr<Program> Parser::parse_program() {
    auto program = std::make_unique<Program>();
    while (cur_token_.type != TokenType::END_OF_FILE) {
        auto stmt = parse_statement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        next_token();
    }
    return program;
}

std::unique_ptr<Statement> Parser::parse_statement() {
    if (cur_token_.type == TokenType::LET || cur_token_.type == TokenType::MUT) {
        return parse_var_declaration_statement();
    }
    return nullptr;
}

std::unique_ptr<VarDeclarationStatement> Parser::parse_var_declaration_statement() {
    auto stmt = std::make_unique<VarDeclarationStatement>();
    stmt->token = cur_token_;
    stmt->is_mutable = cur_token_.type == TokenType::MUT;

    if (peek_token_.type != TokenType::IDENTIFIER) {
        return nullptr;
    }
    next_token();

    auto name = std::make_unique<Identifier>();
    name->token = cur_token_;
    name->value = cur_token_.literal;
    stmt->name = std::move(name);

    if (peek_token_.type != TokenType::ASSIGN) {
        return nullptr;
    }
    next_token();
    next_token();

    stmt->value = parse_expression();

    if (peek_token_.type != TokenType::SEMICOLON) {
        return nullptr;
    }
    next_token();

    return stmt;
}

std::unique_ptr<Expression> Parser::parse_expression() {
    if (cur_token_.type == TokenType::INTEGER) {
        auto lit = std::make_unique<IntegerLiteral>();
        lit->token = cur_token_;
        lit->value = std::stoll(cur_token_.literal);
        return lit;
    }
    return nullptr;
}
