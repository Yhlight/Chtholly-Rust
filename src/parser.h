#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>
#include <map>

enum Precedence {
    LOWEST,
    EQUALS,      // ==
    LESSGREATER, // > or <
    SUM,         // +
    PRODUCT,     // *
    PREFIX,      // -X or !X
    POSTFIX,     // X++
    CALL,        // myFunction(X)
};

class Parser {
public:
    explicit Parser(Lexer& lexer);
    std::unique_ptr<Program> parse_program();

private:
    void next_token();
    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<VarDeclarationStatement> parse_var_declaration_statement();
    std::unique_ptr<FunctionStatement> parse_function_statement();
    std::unique_ptr<WhileStatement> parse_while_statement();
    std::unique_ptr<ForStatement> parse_for_statement();
    std::unique_ptr<DoWhileStatement> parse_do_while_statement();
    std::unique_ptr<BlockStatement> parse_block_statement();
    std::unique_ptr<Expression> parse_expression(Precedence precedence);
    std::unique_ptr<Type> parse_type();
    std::vector<std::pair<std::unique_ptr<Identifier>, std::unique_ptr<Type>>> parse_function_parameters();
    std::vector<std::unique_ptr<Expression>> parse_call_arguments();

    Precedence peek_precedence();
    Precedence cur_precedence();

    Lexer& lexer_;
    Token cur_token_;
    Token peek_token_;

    std::map<TokenType, Precedence> precedences;
};

#endif //CHTHOLLY_PARSER_H
