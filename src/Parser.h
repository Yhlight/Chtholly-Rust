#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(std::vector<Token> tokens);
    std::unique_ptr<BlockStmtAST> parse();

private:
    std::vector<Token> tokens;
    int current_token_idx = 0;

    Token& peek();
    Token& advance();
    bool is_at_end();

    // Parsing methods
    std::unique_ptr<StmtAST> parse_statement();
    std::unique_ptr<StmtAST> parse_variable_declaration();
    std::unique_ptr<StmtAST> parse_function_definition();
    std::unique_ptr<StmtAST> parse_if_statement();
    std::unique_ptr<StmtAST> parse_while_statement();
    std::unique_ptr<StmtAST> parse_expression_statement();
    std::unique_ptr<StmtAST> parse_return_statement();
    std::unique_ptr<BlockStmtAST> parse_block();
    std::unique_ptr<TypeNameAST> parse_type();
    std::unique_ptr<ExprAST> parse_expression();
    std::unique_ptr<ExprAST> parse_primary();
    std::unique_ptr<ExprAST> parse_binary_expression(int min_precedence, std::unique_ptr<ExprAST> lhs);
    std::unique_ptr<ExprAST> parse_call_expression(std::unique_ptr<ExprAST> callee);
    int get_token_precedence();
};

#endif // CHTHOLLY_PARSER_H
