#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>

class Parser {
public:
    explicit Parser(Lexer& lexer);
    std::unique_ptr<Program> parse_program();

private:
    void next_token();
    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<LetStatement> parse_let_statement();
    std::unique_ptr<Expression> parse_expression();

    Lexer& lexer_;
    Token cur_token_;
    Token peek_token_;
};

#endif //CHTHOLLY_PARSER_H
