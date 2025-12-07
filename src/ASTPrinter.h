#ifndef CHTHOLLY_AST_PRINTER_H
#define CHTHOLLY_AST_PRINTER_H

#include "AST.h"
#include <string>
#include <vector>

namespace Chtholly {

class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const Expr& expr);
    std::string print(const Stmt& stmt);

    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;

    void visitBlockStmt(const BlockStmt& stmt) override;
    void visitExpressionStmt(const ExpressionStmt& stmt) override;
    void visitFunctionStmt(const FunctionStmt& stmt) override;
    void visitIfStmt(const IfStmt& stmt) override;
    void visitReturnStmt(const ReturnStmt& stmt) override;
    void visitVarDeclStmt(const VarDeclStmt& stmt) override;
    void visitWhileStmt(const WhileStmt& stmt) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);

    std::string m_result;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_PRINTER_H
