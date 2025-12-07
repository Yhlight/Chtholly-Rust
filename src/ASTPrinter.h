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

    void visitExpressionStmt(const ExpressionStmt& stmt) override;
    void visitVarDeclStmt(const VarDeclStmt& stmt) override;

private:
    template<typename... Args>
    std::string parenthesize(const std::string& name, const Args&... exprs);

    std::string m_result;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_PRINTER_H
