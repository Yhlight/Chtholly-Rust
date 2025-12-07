#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "Token.h"
#include <memory>
#include <vector>
#include <any>

namespace Chtholly {

// Forward declarations
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;

class ExprVisitor {
public:
    virtual std::any visitBinaryExpr(const BinaryExpr& expr) = 0;
    virtual std::any visitGroupingExpr(const GroupingExpr& expr) = 0;
    virtual std::any visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual std::any visitUnaryExpr(const UnaryExpr& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

struct GroupingExpr : Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct LiteralExpr : Expr {
    LiteralExpr(std::any value) : value(value) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }

    const std::any value;
};

struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};

// Forward declarations for Stmt
struct ExpressionStmt;
struct VarDeclStmt;

class StmtVisitor {
public:
    virtual void visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual void visitVarDeclStmt(const VarDeclStmt& stmt) = 0;
};


class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};


struct ExpressionStmt : Stmt {
    ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitExpressionStmt(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct VarDeclStmt : Stmt {
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(name), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitVarDeclStmt(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> initializer;
};


} // namespace Chtholly

#endif // CHTHOLLY_AST_H
