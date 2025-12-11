#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "../Lexer/Lexer.h"
#include "../AST/ASTNode.h"
#include "../AST/StmtAST.h"
#include <memory>
#include <vector>

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<StmtAST> parseLetStatement();

private:
    Lexer& lexer;
    Token currentToken;

    void eat(TokenType type);
};

#endif //CHTHOLLY_PARSER_H
