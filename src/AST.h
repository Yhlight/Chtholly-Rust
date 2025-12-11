#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "Token.h"
#include <memory>
#include <vector>

// Forward declarations for Visitor
class NumberExpr;
class VariableExpr;
class BinaryExpr;
class LetStmt;
class ReturnStmt;
class FnDecl;
class ExpressionStmt;
class IfStmt;
class BlockStmt;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(NumberExpr& expr) = 0;
    virtual void visit(VariableExpr& expr) = 0;
    virtual void visit(BinaryExpr& expr) = 0;
    virtual void visit(LetStmt& stmt) = 0;
    virtual void visit(ReturnStmt& stmt) = 0;
    virtual void visit(FnDecl& stmt) = 0;
    virtual void visit(ExpressionStmt& stmt) = 0;
    virtual void visit(IfStmt& stmt) = 0;
    virtual void visit(BlockStmt& stmt) = 0;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class Expr : public ASTNode {};

class Stmt : public ASTNode {};

class NumberExpr : public Expr {
public:
    Token token;
    explicit NumberExpr(Token token) : token(std::move(token)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class VariableExpr : public Expr {
public:
    Token token;
    explicit VariableExpr(Token token) : token(std::move(token)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class LetStmt : public Stmt {
public:
    Token name;
    std::unique_ptr<Expr> initializer;
    LetStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class ReturnStmt : public Stmt {
public:
    Token keyword;
    std::unique_ptr<Expr> value;
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class FnDecl : public Stmt {
public:
    Token name;
    std::vector<Token> params;
    std::vector<std::unique_ptr<Stmt>> body;
    FnDecl(Token name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class ExpressionStmt : public Stmt {
public:
    std::unique_ptr<Expr> expression;
    explicit ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class BlockStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> statements;
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    void accept(Visitor& visitor) override { visitor.visit(*this); }
};

#endif // CHTHOLLY_AST_H
