#include "ASTPrinter.h"

namespace Chtholly
{
    std::string ASTPrinter::Print(const std::vector<std::unique_ptr<Stmt>>& statements)
    {
        std::string result;
        for (const auto& stmt : statements)
        {
            result += stmt->Accept(*this) + "\n";
        }
        return result;
    }

    std::string ASTPrinter::Print(const Expr& expr)
    {
        return const_cast<Expr&>(expr).Accept(*this);
    }

    std::string ASTPrinter::Print(const Stmt& stmt)
    {
        return const_cast<Stmt&>(stmt).Accept(*this);
    }

    std::string ASTPrinter::Visit(LiteralExpr& expr)
    {
        return expr.value.GetValue();
    }

    std::string ASTPrinter::Visit(UnaryExpr& expr)
    {
        return Parenthesize(expr.op.GetValue(), *expr.right);
    }

    std::string ASTPrinter::Visit(BinaryExpr& expr)
    {
        return Parenthesize(expr.op.GetValue(), *expr.left, *expr.right);
    }

    std::string ASTPrinter::Visit(GroupingExpr& expr)
    {
        return Parenthesize("group", *expr.expression);
    }

    std::string ASTPrinter::Visit(LetDeclStmt& stmt)
    {
        std::string result = "(let";
        if (stmt.is_mutable)
        {
            result += " mut";
        }
        result += " " + stmt.name.GetValue();
        if (stmt.initializer)
        {
            result += " = " + Print(*stmt.initializer);
        }
        result += ")";
        return result;
    }

    std::string ASTPrinter::Visit(ExpressionStmt& stmt)
    {
        return "(expr " + Print(*stmt.expression) + ")";
    }

    std::string ASTPrinter::Parenthesize(const std::string& name, const Expr& expr)
    {
        return "(" + name + " " + Print(expr) + ")";
    }

    std::string ASTPrinter::Parenthesize(const std::string& name, const Expr& left, const Expr& right)
    {
        return "(" + name + " " + Print(left) + " " + Print(right) + ")";
    }
}
