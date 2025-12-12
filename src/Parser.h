#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "AST.h"
#include "Lexer.h"
#include <vector>

namespace Chtholly {

class Parser {
public:
    Parser(Lexer& lexer);

    std::vector<std::unique_ptr<StmtAST>> parse();

private:
    std::unique_ptr<StmtAST> parseStatement();
    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<StmtAST> parseVarDeclStatement();
    std::unique_ptr<ExprAST> parsePrimary();
    std::unique_ptr<ExprAST> parseNumberExpression();

    void consume(TokenType expected);

    Lexer& lexer;
    Token currentToken;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
