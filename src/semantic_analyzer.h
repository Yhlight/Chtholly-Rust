#ifndef CHTHOLLY_SEMANTIC_ANALYZER_H
#define CHTHOLLY_SEMANTIC_ANALYZER_H

#include "ast.h"
#include "symbol_table.h"
#include <iostream>
#include <set>
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
        void visit(const GetExpr& expr) override;
        void visit(const SetExpr& expr) override;
        void visit(const StructInitializerExpr& expr) override;

        void visit(const ExpressionStmt& stmt) override;
        void visit(const LetStmt& stmt) override;
        void visit(const BlockStmt& stmt) override;
        void visit(const IfStmt& stmt) override;
        void visit(const WhileStmt& stmt) override;
        void visit(const ForStmt& stmt) override;
        void visit(const FunctionStmt& stmt) override;
        void visit(const ReturnStmt& stmt) override;
        void visit(const SwitchStmt& stmt) override;
        void visit(const CaseStmt& stmt) override;
        void visit(const BreakStmt& stmt) override;
        void visit(const ContinueStmt& stmt) override;
        void visit(const FallthroughStmt& stmt) override;
        void visit(const StructStmt& stmt) override;

    private:
        enum class FunctionType {
            NONE,
            FUNCTION
        };

        enum class LoopType {
            NONE,
            LOOP,
            SWITCH
        };

        void check(const std::shared_ptr<Expr>& expr);
        void check(const std::shared_ptr<Stmt>& stmt);
        void checkForDanglingReference(const SymbolInfo* lhsInfo, const std::shared_ptr<Expr>& rhsExpr);
        std::string typeOf(const std::shared_ptr<Expr>& expr);

        SymbolTable symbolTable;
        std::set<std::string> copyTypes;
        FunctionType currentFunction = FunctionType::NONE;
        LoopType currentLoop = LoopType::NONE;
        std::string currentReturnType;

    };

} // namespace Chtholly

#endif // CHTHOLLY_SEMANTIC_ANALYZER_H
