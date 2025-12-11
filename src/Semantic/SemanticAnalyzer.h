#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "../AST/ASTNode.h"
#include "../AST/ProgramAST.h"
#include "../AST/StmtAST.h"
#include "../AST/ExprAST.h"
#include "SymbolTable.h"
#include "../Type/Type.h"

class SemanticAnalyzer {
public:
    void visit(const ProgramAST& node);
    void visit(const LetStmtAST& node);
    Type visit(const ExprAST& node);
    Type visit(const NumberExprAST& node);
    Type visit(const FloatExprAST& node);

private:
    SymbolTable symbolTable;
};

#endif //CHTHOLLY_SEMANTICANALYZER_H
