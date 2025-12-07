#include "ASTPrinter.h"
#include <sstream>

namespace Chtholly {

std::string ASTPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::string ASTPrinter::print(const Stmt& stmt) {
    stmt.accept(*this);
    return m_result;
}

std::any ASTPrinter::visitBinaryExpr(const BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, *expr.left, *expr.right);
}

std::any ASTPrinter::visitGroupingExpr(const GroupingExpr& expr) {
    return parenthesize("group", *expr.expression);
}

std::any ASTPrinter::visitLiteralExpr(const LiteralExpr& expr) {
    if (expr.value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(expr.value);
    }
    if (expr.value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(expr.value));
    }
    if (expr.value.type() == typeid(bool)) {
        return std::any_cast<bool>(expr.value) ? "true" : "false";
    }
    return "nil";
}

std::any ASTPrinter::visitUnaryExpr(const UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, *expr.right);
}

void ASTPrinter::visitExpressionStmt(const ExpressionStmt& stmt) {
    m_result = print(*stmt.expression);
}

void ASTPrinter::visitVarDeclStmt(const VarDeclStmt& stmt) {
    std::stringstream builder;
    builder << "(var " << stmt.name.lexeme;
    if (stmt.initializer) {
        builder << " = " << print(*stmt.initializer);
    }
    builder << ")";
    m_result = builder.str();
}

template<typename... Args>
std::string ASTPrinter::parenthesize(const std::string& name, const Args&... exprs) {
    std::stringstream builder;
    builder << "(" << name;
    ((builder << " " << std::any_cast<std::string>(exprs.accept(*this))), ...);
    builder << ")";
    return builder.str();
}

} // namespace Chtholly
