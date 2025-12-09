#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "AST.h"
#include "SymbolTable.h"
#include <memory>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    void analyze(const BlockStmtAST& ast);

private:
    SymbolTable symbolTable;

    void visit(const ASTNode& node);
    void visit(const VarDeclStmtAST& node);
    void visit(const FunctionDeclAST& node);
    void visit(const BlockStmtAST& node);
    void visit(const BinaryExprAST& node);
    void visit(const NumberExprAST& node);
    void visit(const VariableExprAST& node);
    void visit(const TypeNameAST& node);
};

#endif // CHTHOLLY_SEMANTICANALYZER_H
