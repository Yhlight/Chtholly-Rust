#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "AST.h"
#include "SymbolTable.h"
#include <vector>
#include <memory>

class SemanticAnalyzer : public Visitor {
public:
    void analyze(const std::vector<std::unique_ptr<Stmt>>& statements);

    void visit(NumberExpr& expr) override;
    void visit(VariableExpr& expr) override;
    void visit(BinaryExpr& expr) override;
    void visit(LetStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;
    void visit(FnDecl& stmt) override;
    void visit(ExpressionStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(BlockStmt& stmt) override;

private:
    std::shared_ptr<SymbolTable> currentScope;
};

#endif //CHTHOLLY_SEMANTICANALYZER_H
