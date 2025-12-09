#ifndef CHTHOLLY_SEMANTIC_ANALYZER_H
#define CHTHOLLY_SEMANTIC_ANALYZER_H

#include "ast.h"
#include "symbol_table.h"
#include <iostream>
#include <unordered_set>

namespace Chtholly
{

    class SemanticAnalyzer : public ExprVisitor, public StmtVisitor
    {
    public:
        SemanticAnalyzer();

        void analyze(const std::vector<std::shared_ptr<Stmt>>& statements);

        void visit(const BinaryExpr& expr) override;
        void visit(const UnaryExpr& expr) override;
        void visit(const LiteralExpr& expr) override;
        void visit(const VariableExpr& expr) override;
        void visit(const AssignExpr& expr) override;
        void visit(const CallExpr& expr) override;

        void visit(const ExpressionStmt& stmt) override;
        void visit(const LetStmt& stmt) override;
        void visit(const BlockStmt& stmt) override;
        void visit(const IfStmt& stmt) override;
        void visit(const WhileStmt& stmt) override;
        void visit(const ForStmt& stmt) override;
        void visit(const FunctionStmt& stmt) override;
        void visit(const ReturnStmt& stmt) override;

    private:
        void check(const std::shared_ptr<Expr>& expr);
        void check(const std::shared_ptr<Stmt>& stmt);
        std::string type(const std::shared_ptr<Expr>& expr);

        SymbolTable symbolTable;
        std::unordered_set<std::string> copyTypes;

        struct FunctionInfo {
            std::string returnType;
        };

        FunctionInfo* currentFunction = nullptr;

    };

} // namespace Chtholly

#endif // CHTHOLLY_SEMANTIC_ANALYZER_H
