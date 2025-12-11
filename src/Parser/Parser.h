#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "../Lexer/Lexer.h"
#include "../AST/ASTNode.h"
#include "../AST/StmtAST.h"
#include "../AST/ProgramAST.h"
#include "../Type/Type.h"
#include <memory>
#include <vector>

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<ProgramAST> parse();

private:
    Lexer& lexer;
    Token currentToken;

    void eat(TokenType type);
    std::unique_ptr<StmtAST> parseStatement();
    std::unique_ptr<StmtAST> parseLetStatement();
    Type parseType();
    std::unique_ptr<ExprAST> parseExpression();
};

#endif //CHTHOLLY_PARSER_H
