#ifndef CHTHOLLY_SEMANTIC_ANALYZER_H
#define CHTHOLLY_SEMANTIC_ANALYZER_H

#include "ast.h"
#include "symbol_table.h"
#include <iostream>
#include <unordered_set>

namespace Chtholly
{
    enum class SymbolState
    {
        Valid,
        Moved
    };

    struct SymbolInfo
    {
        std::string type;
        bool isMutable;
        SymbolState state;
        int sharedBorrowCount = 0;
        bool mutableBorrow = false;
    };

    struct FunctionInfo
    {
        int arity;
    };

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
        void visit(const ReferenceExpr& expr) override;
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

        SymbolTable<SymbolInfo> symbolTable;
        SymbolTable<FunctionInfo> functionTable;
        std::unordered_set<std::string> copyTypes;
        std::vector<std::vector<std::string>> borrowedSymbols;
        bool inFunction = false;

    };

} // namespace Chtholly

#endif // CHTHOLLY_SEMANTIC_ANALYZER_H
