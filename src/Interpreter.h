#pragma once

#include "AST.h"
#include "Environment.h"
#include "Object.h"

class Interpreter : public StmtVisitor, public ExprVisitor {
public:
    Interpreter();
    void interpret(const Program& program);

    // Statement visitor methods
    void visitVarDeclStmt(VarDeclStmt& stmt) override;

    // Expression visitor methods
    void visitBinaryExpr(BinaryExpr& expr) override;
    void visitUnaryExpr(UnaryExpr& expr) override;
    void visitGroupingExpr(GroupingExpr& expr) override;
    void visitLiteralExpr(LiteralExpr& expr) override;
    void visitVariableExpr(VariableExpr& expr) override;

    std::shared_ptr<Environment> environment;

private:
    Object evaluate(Expr& expr);
    void execute(Stmt& stmt);

    Object lastEvaluated;
};
