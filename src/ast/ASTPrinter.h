#ifndef CHTHOLLY_AST_PRINTER_H
#define CHTHOLLY_AST_PRINTER_H

#include "Expr.h"
#include "Stmt.h"
#include <string>
#include <vector>
#include <memory>

namespace Chtholly
{
    class ASTPrinter : public ExprVisitor<std::string>, public StmtVisitor<std::string>
    {
    public:
        std::string Print(const std::vector<std::unique_ptr<Stmt>>& statements);
        std::string Print(const Expr& expr);
        std::string Print(const Stmt& stmt);

        std::string Visit(LiteralExpr& expr) override;
        std::string Visit(UnaryExpr& expr) override;
        std::string Visit(BinaryExpr& expr) override;

        std::string Visit(LetDeclStmt& stmt) override;
        std::string Visit(ExpressionStmt& stmt) override;

    private:
        std::string Parenthesize(const std::string& name, const Expr& expr);
        std::string Parenthesize(const std::string& name, const Expr& left, const Expr& right);
    };
}

#endif //CHTHOLLY_AST_PRINTER_H
