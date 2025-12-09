#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "token.h"
#include <memory>
#include <vector>

namespace Chtholly
{

    // Forward declarations
    struct BinaryExpr;
    struct UnaryExpr;
    struct LiteralExpr;
    struct VariableExpr;
    struct AssignExpr;
    struct ReferenceExpr;
    struct CallExpr;

    // Visitor for Expressions
    class ExprVisitor
    {
    public:
        virtual ~ExprVisitor() = default;
        virtual void visit(const BinaryExpr& expr) = 0;
        virtual void visit(const UnaryExpr& expr) = 0;
        virtual void visit(const LiteralExpr& expr) = 0;
        virtual void visit(const VariableExpr& expr) = 0;
        virtual void visit(const AssignExpr& expr) = 0;
        virtual void visit(const ReferenceExpr& expr) = 0;
        virtual void visit(const CallExpr& expr) = 0;
    };

    // Base class for all expressions
    struct Expr
    {
        virtual ~Expr() = default;
        virtual void accept(ExprVisitor& visitor) const = 0;
    };

    // Concrete Expression nodes
    struct BinaryExpr : Expr
    {
        std::shared_ptr<Expr> left;
        Token op;
        std::shared_ptr<Expr> right;

        BinaryExpr(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
            : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct CallExpr : Expr
    {
        std::shared_ptr<Expr> callee;
        Token paren;
        std::vector<std::shared_ptr<Expr>> arguments;

        CallExpr(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
            : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct UnaryExpr : Expr
    {
        Token op;
        std::shared_ptr<Expr> right;
        bool isMutable;

        UnaryExpr(Token op, std::shared_ptr<Expr> right, bool isMutable = false)
            : op(std::move(op)), right(std::move(right)), isMutable(isMutable) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct LiteralExpr : Expr
    {
        std::variant<std::monostate, int, double, std::string, char, bool> value;

        LiteralExpr(std::variant<std::monostate, int, double, std::string, char, bool> value)
            : value(std::move(value)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct VariableExpr : Expr
    {
        Token name;

        VariableExpr(Token name) : name(std::move(name)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct AssignExpr : Expr
    {
        Token name;
        std::shared_ptr<Expr> value;

        AssignExpr(Token name, std::shared_ptr<Expr> value)
            : name(std::move(name)), value(std::move(value)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct ReferenceExpr : Expr
    {
        Token op;
        std::shared_ptr<Expr> expression;
        bool isMutable;

        ReferenceExpr(Token op, std::shared_ptr<Expr> expression, bool isMutable)
            : op(std::move(op)), expression(std::move(expression)), isMutable(isMutable) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    // Forward declarations for Statements
    struct ExpressionStmt;
    struct LetStmt;
    struct BlockStmt;
    struct IfStmt;
    struct WhileStmt;
    struct ForStmt;
    struct FunctionStmt;
    struct ReturnStmt;

    // Visitor for Statements
    class StmtVisitor
    {
    public:
        virtual ~StmtVisitor() = default;
        virtual void visit(const ExpressionStmt& stmt) = 0;
        virtual void visit(const LetStmt& stmt) = 0;
        virtual void visit(const BlockStmt& stmt) = 0;
        virtual void visit(const IfStmt& stmt) = 0;
        virtual void visit(const WhileStmt& stmt) = 0;
        virtual void visit(const ForStmt& stmt) = 0;
        virtual void visit(const FunctionStmt& stmt) = 0;
        virtual void visit(const ReturnStmt& stmt) = 0;
    };

    // Base class for all statements
    struct Stmt
    {
        virtual ~Stmt() = default;
        virtual void accept(StmtVisitor& visitor) const = 0;
    };

    // Concrete Statement nodes
    struct ExpressionStmt : Stmt
    {
        std::shared_ptr<Expr> expression;

        ExpressionStmt(std::shared_ptr<Expr> expression)
            : expression(std::move(expression)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct LetStmt : Stmt
    {
        Token name;
        Token type;
        std::shared_ptr<Expr> initializer;
        bool isMutable;

        LetStmt(Token name, Token type, std::shared_ptr<Expr> initializer, bool isMutable)
            : name(std::move(name)), type(std::move(type)), initializer(std::move(initializer)), isMutable(isMutable) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct BlockStmt : Stmt
    {
        std::vector<std::shared_ptr<Stmt>> statements;

        BlockStmt(std::vector<std::shared_ptr<Stmt>> statements)
            : statements(std::move(statements)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct IfStmt : Stmt
    {
        std::shared_ptr<Expr> condition;
        std::shared_ptr<Stmt> thenBranch;
        std::shared_ptr<Stmt> elseBranch;

        IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
            : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct WhileStmt : Stmt
    {
        std::shared_ptr<Expr> condition;
        std::shared_ptr<Stmt> body;

        WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
            : condition(std::move(condition)), body(std::move(body)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct ForStmt : Stmt
    {
        std::shared_ptr<Stmt> initializer;
        std::shared_ptr<Expr> condition;
        std::shared_ptr<Expr> increment;
        std::shared_ptr<Stmt> body;

        ForStmt(std::shared_ptr<Stmt> initializer, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> increment, std::shared_ptr<Stmt> body)
            : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct FunctionStmt : Stmt
    {
        Token name;
        std::vector<Token> params;
        std::vector<std::shared_ptr<Stmt>> body;

        FunctionStmt(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
            : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct ReturnStmt : Stmt
    {
        Token keyword;
        std::shared_ptr<Expr> value;

        ReturnStmt(Token keyword, std::shared_ptr<Expr> value)
            : keyword(std::move(keyword)), value(std::move(value)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
