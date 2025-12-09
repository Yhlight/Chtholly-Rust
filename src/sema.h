#ifndef CHTHOLLY_SEMA_H
#define CHTHOLLY_SEMA_H

#include "ast.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Chtholly
{

    enum class SymbolState
    {
        DECLARED,
        INITIALIZED,
        MOVED,
        BORROWED_IMMUTABLY,
        BORROWED_MUTABLY
    };

    struct Symbol
    {
        std::string name;
        std::string type;
        bool isMutable;
        SymbolState state;
        int scopeLevel;
    };

    class SymbolTable
    {
    public:
        SymbolTable();
        void enterScope();
        void exitScope();
        bool define(const std::string& name, const std::string& type, bool isMutable);
        std::shared_ptr<Symbol> lookup(const std::string& name);
        void setState(const std::string& name, SymbolState state);

    private:
        std::vector<std::unordered_map<std::string, std::shared_ptr<Symbol>>> scopes;
        int currentScopeLevel;
    };


    class Sema : public StmtVisitor, public ExprVisitor
    {
    public:
        Sema();
        void analyze(const std::vector<std::shared_ptr<Stmt>>& statements);

        // Statement visitors
        void visit(const ExpressionStmt& stmt) override;
        void visit(const LetStmt& stmt) override;
        void visit(const BlockStmt& stmt) override;
        void visit(const IfStmt& stmt) override;
        void visit(const WhileStmt& stmt) override;
        void visit(const ForStmt& stmt) override;


        // Expression visitors
        void visit(const BinaryExpr& expr) override;
        void visit(const UnaryExpr& expr) override;
        void visit(const LiteralExpr& expr) override;
        void visit(const VariableExpr& expr) override;
        void visit(const AssignExpr& expr) override;


    private:
        void analyze(const std::shared_ptr<Stmt>& stmt);
        void analyze(const std::shared_ptr<Expr>& expr);

        SymbolTable symbolTable;
    };

} // namespace Chtholly

#endif // CHTHOLLY_SEMA_H
