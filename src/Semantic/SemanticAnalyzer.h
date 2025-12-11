#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "../AST/ASTNode.h"
#include "../AST/ProgramAST.h"
#include "../AST/StmtAST.h"
#include "SymbolTable.h"

class SemanticAnalyzer {
public:
    void visit(const ProgramAST& node);
    void visit(const LetStmtAST& node);

private:
    SymbolTable symbolTable;
};

#endif //CHTHOLLY_SEMANTICANALYZER_H
