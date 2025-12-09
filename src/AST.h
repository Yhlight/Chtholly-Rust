#pragma once

#include "Token.h"
#include <memory>
#include <vector>
#include <string>

// Forward declarations
struct BinaryExpr;
struct UnaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct VariableExpr;
struct Stmt;
struct VarDeclStmt;

// Visitor pattern for expressions
struct ExprVisitor {
    virtual ~ExprVisitor() = default;
    virtual void visitBinaryExpr(BinaryExpr& expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr& expr) = 0;
    virtual void visitGroupingExpr(GroupingExpr& expr) = 0;
    virtual void visitLiteralExpr(LiteralExpr& expr) = 0;
    virtual void visitVariableExpr(VariableExpr& expr) = 0;
};

struct Expr {
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) override {
        visitor.visitBinaryExpr(*this);
    }
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) override {
        visitor.visitUnaryExpr(*this);
    }
    Token op;
    std::unique_ptr<Expr> right;
};

struct GroupingExpr : Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    void accept(ExprVisitor& visitor) override {
        visitor.visitGroupingExpr(*this);
    }
    std::unique_ptr<Expr> expression;
};

struct LiteralExpr : Expr {
    LiteralExpr(Token token) : token(std::move(token)) {}
    void accept(ExprVisitor& visitor) override {
        visitor.visitLiteralExpr(*this);
    }
    Token token;
};

struct VariableExpr : Expr {
    VariableExpr(Token name) : name(std::move(name)) {}
    void accept(ExprVisitor& visitor) override {
        visitor.visitVariableExpr(*this);
    }
    Token name;
};

// Visitor pattern for statements
struct StmtVisitor {
    virtual ~StmtVisitor() = default;
    virtual void visitVarDeclStmt(VarDeclStmt& stmt) = 0;
};

struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

struct VarDeclStmt : Stmt {
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(std::move(name)), initializer(std::move(initializer)), isMutable(isMutable) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitVarDeclStmt(*this);
    }

    Token name;
    std::unique_ptr<Expr> initializer;
    bool isMutable;
};

using Program = std::vector<std::unique_ptr<Stmt>>;
