#pragma once

#include "Token.h"
#include <memory>
#include <vector>
#include <string>

// Forward declarations
struct Expr;
struct Stmt;
struct LiteralExpr;
struct VarDeclStmt;

// Visitor pattern for expressions
struct ExprVisitor {
    virtual ~ExprVisitor() = default;
    virtual void visitLiteralExpr(LiteralExpr& expr) = 0;
};

struct Expr {
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

struct LiteralExpr : Expr {
    LiteralExpr(Token token) : token(std::move(token)) {}
    void accept(ExprVisitor& visitor) override {
        visitor.visitLiteralExpr(*this);
    }
    Token token;
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
