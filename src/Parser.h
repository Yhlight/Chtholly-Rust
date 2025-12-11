#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "AST.h"
#include "Lexer.h"
#include <memory>
#include <map>

namespace chtholly {

class Parser {
public:
    Parser(Lexer& lexer);

    // Main parsing loop
    std::unique_ptr<FunctionAST> handleDefinition();
    std::unique_ptr<FunctionAST> handleTopLevelExpression(); // For testing simple expressions

private:
    Lexer& m_lexer;
    Token m_currentToken;

    // Advances the lexer to the next token
    Token getNextToken();

    // Error handling
    std::unique_ptr<ExprAST> logError(const char* str);
    std::unique_ptr<PrototypeAST> logErrorP(const char* str);

    // Parsing routines
    std::unique_ptr<Type> parseType();
    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<ExprAST> parseNumberExpr();
    std::unique_ptr<ExprAST> parseParenExpr();
    std::unique_ptr<ExprAST> parseIdentifierExpr();
    std::unique_ptr<ExprAST> parseIfExpr();
    std::unique_ptr<ExprAST> parseLetExpr();
    std::unique_ptr<ExprAST> parseStructDef();
    std::unique_ptr<ExprAST> parsePrimary();
    std::unique_ptr<ExprAST> parseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> lhs);
    std::unique_ptr<PrototypeAST> parsePrototype();
    std::unique_ptr<FunctionAST> parseDefinition();
    std::unique_ptr<FunctionAST> parseTopLevelExpr();
};

} // namespace chtholly

#endif // CHTHOLLY_PARSER_H
