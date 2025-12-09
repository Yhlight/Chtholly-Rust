#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "AST.h"
#include "SymbolTable.h"
#include "TypeResolver.h"
#include <memory>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    void analyze(BlockStmtAST& ast);

private:
    SymbolTable symbolTable;
    TypeResolver typeResolver;

    std::shared_ptr<Type> visit(ASTNode& node);
    std::shared_ptr<Type> visit(VarDeclStmtAST& node);
    std::shared_ptr<Type> visit(FunctionDeclAST& node);
    std::shared_ptr<Type> visit(BlockStmtAST& node);
    std::shared_ptr<Type> visit(BinaryExprAST& node);
    std::shared_ptr<Type> visit(NumberExprAST& node);
    std::shared_ptr<Type> visit(StringExprAST& node);
    std::shared_ptr<Type> visit(VariableExprAST& node);
    std::shared_ptr<Type> visit(TypeNameAST& node);
};

#endif // CHTHOLLY_SEMANTICANALYZER_H
