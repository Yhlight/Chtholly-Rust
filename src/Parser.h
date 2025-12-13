#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "AST.h"
#include "Lexer.h"
#include "Type.h"
#include <string>
#include <vector>

namespace Chtholly {

class Parser {
public:
    Parser(Lexer& lexer);

    std::vector<std::unique_ptr<FunctionAST>> parse();

    const std::vector<std::string>& getErrors() const { return m_errors; }

private:
    void logError(const std::string& message);
    void synchronize();

    std::unique_ptr<StmtAST> parseStatement();
    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<StmtAST> parseVarDeclStatement();
    std::unique_ptr<ExprAST> parsePrimary();
    std::unique_ptr<ExprAST> parseNumberExpression();
    std::unique_ptr<ExprAST> parseBooleanExpression();
    std::unique_ptr<ExprAST> parseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
    std::unique_ptr<ExprAST> parseIdentifierExpression();
    std::unique_ptr<PrototypeAST> parsePrototype();
    std::unique_ptr<FunctionAST> parseFunctionDefinition();
    Type* parseType();
    int getTokPrecedence();
    std::unique_ptr<StmtAST> parseReturnStatement();
    std::unique_ptr<StmtAST> parseExpressionStatement();
    std::unique_ptr<StmtAST> parseIfStatement();
    std::unique_ptr<StmtAST> parseWhileStatement();
    std::unique_ptr<StmtAST> parseForStatement();
    std::unique_ptr<StmtAST> parseDoWhileStatement();
    std::unique_ptr<StmtAST> parseSwitchStatement();

    void consume(TokenType expected);

    Lexer& lexer;
    Token currentToken;
    std::vector<std::string> m_errors;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
