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
    return std::make_any<std::string>(parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()}));
}

std::any ASTPrinter::visitGroupingExpr(const GroupingExpr& expr) {
    return std::make_any<std::string>(parenthesize("group", {expr.expression.get()}));
}

std::any ASTPrinter::visitLiteralExpr(const LiteralExpr& expr) {
    if (expr.value.type() == typeid(std::string)) {
        return expr.value;
    }
    if (expr.value.type() == typeid(double)) {
        return std::make_any<std::string>(std::to_string(std::any_cast<double>(expr.value)));
    }
    if (expr.value.type() == typeid(bool)) {
        return std::make_any<std::string>(std::any_cast<bool>(expr.value) ? "true" : "false");
    }
    return std::make_any<std::string>("nil");
}

std::any ASTPrinter::visitUnaryExpr(const UnaryExpr& expr) {
    return std::make_any<std::string>(parenthesize(expr.op.lexeme, {expr.right.get()}));
}

std::any ASTPrinter::visitVariableExpr(const VariableExpr& expr) {
    return std::make_any<std::string>(expr.name.lexeme);
}

void ASTPrinter::visitBlockStmt(const BlockStmt& stmt) {
    std::stringstream builder;
    builder << "(block";
    for (const auto& statement : stmt.statements) {
        builder << " " << print(*statement);
    }
    builder << ")";
    m_result = builder.str();
}

void ASTPrinter::visitExpressionStmt(const ExpressionStmt& stmt) {
    m_result = print(*stmt.expression);
}

void ASTPrinter::visitFunctionStmt(const FunctionStmt& stmt) {
    std::stringstream builder;
    builder << "(fun " << stmt.name.lexeme << " (";
    for (const auto& param : stmt.params) {
        builder << " " << param.lexeme;
    }
    builder << " )";
    for (const auto& statement : stmt.body) {
        builder << " " << print(*statement);
    }
    builder << ")";
    m_result = builder.str();
}

void ASTPrinter::visitIfStmt(const IfStmt& stmt) {
    std::stringstream builder;
    builder << "(if " << print(*stmt.condition) << " " << print(*stmt.thenBranch);
    if (stmt.elseBranch) {
        builder << " " << print(*stmt.elseBranch);
    }
    builder << ")";
    m_result = builder.str();
}

void ASTPrinter::visitReturnStmt(const ReturnStmt& stmt) {
    std::stringstream builder;
    builder << "(return";
    if (stmt.value) {
        builder << " " << print(*stmt.value);
    }
    builder << ")";
    m_result = builder.str();
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

void ASTPrinter::visitWhileStmt(const WhileStmt& stmt) {
    std::stringstream builder;
    builder << "(while " << print(*stmt.condition) << " " << print(*stmt.body) << ")";
    m_result = builder.str();
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    std::stringstream builder;
    builder << "(" << name;
    for (const auto& expr : exprs) {
        builder << " " << print(*expr);
    }
    builder << ")";
    return builder.str();
}

} // namespace Chtholly
