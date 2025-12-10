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
    struct CallExpr;
    struct GetExpr;
    struct SetExpr;
    struct StructInitializerExpr;
    struct ThisExpr;

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
        virtual void visit(const CallExpr& expr) = 0;
        virtual void visit(const GetExpr& expr) = 0;
        virtual void visit(const SetExpr& expr) = 0;
        virtual void visit(const StructInitializerExpr& expr) = 0;
        virtual void visit(const ThisExpr& expr) = 0;
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

    struct CallExpr : Expr
    {
        std::shared_ptr<Expr> callee;
        Token paren; // For error reporting
        std::vector<std::shared_ptr<Expr>> arguments;

        CallExpr(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
            : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct GetExpr : Expr
    {
        std::shared_ptr<Expr> object;
        Token name;

        GetExpr(std::shared_ptr<Expr> object, Token name)
            : object(std::move(object)), name(std::move(name)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct SetExpr : Expr
    {
        std::shared_ptr<Expr> object;
        Token name;
        std::shared_ptr<Expr> value;

        SetExpr(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
            : object(std::move(object)), name(std::move(name)), value(std::move(value)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct StructInitializerExpr : Expr
    {
        Token name;
        std::vector<std::pair<Token, std::shared_ptr<Expr>>> initializers;

        StructInitializerExpr(Token name, std::vector<std::pair<Token, std::shared_ptr<Expr>>> initializers)
            : name(std::move(name)), initializers(std::move(initializers)) {}

        void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct ThisExpr : Expr
    {
        Token keyword;

        ThisExpr(Token keyword) : keyword(std::move(keyword)) {}

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
    struct SwitchStmt;
    struct CaseStmt;
    struct BreakStmt;
    struct ContinueStmt;
    struct FallthroughStmt;
    struct StructStmt;
    struct ClassStmt;
    struct EnumStmt;

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
        virtual void visit(const SwitchStmt& stmt) = 0;
        virtual void visit(const CaseStmt& stmt) = 0;
        virtual void visit(const BreakStmt& stmt) = 0;
        virtual void visit(const ContinueStmt& stmt) = 0;
        virtual void visit(const FallthroughStmt& stmt) = 0;
        virtual void visit(const StructStmt& stmt) = 0;
        virtual void visit(const ClassStmt& stmt) = 0;
        virtual void visit(const EnumStmt& stmt) = 0;
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

    struct FunctionStmt : Stmt
    {
        Token name;
        std::vector<Token> parameters;
        std::vector<Token> parameterTypes;
        Token returnType;
        std::shared_ptr<BlockStmt> body;

        FunctionStmt(Token name, std::vector<Token> parameters, std::vector<Token> parameterTypes, Token returnType, std::shared_ptr<BlockStmt> body)
            : name(std::move(name)), parameters(std::move(parameters)), parameterTypes(std::move(parameterTypes)), returnType(std::move(returnType)), body(std::move(body)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct ReturnStmt : Stmt
    {
        Token keyword; // For error reporting
        std::shared_ptr<Expr> value;

        ReturnStmt(Token keyword, std::shared_ptr<Expr> value)
            : keyword(std::move(keyword)), value(std::move(value)) {}

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

    struct CaseStmt : Stmt
    {
        std::shared_ptr<Expr> condition;
        std::shared_ptr<Stmt> body;

        CaseStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
            : condition(std::move(condition)), body(std::move(body)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct SwitchStmt : Stmt
    {
        std::shared_ptr<Expr> condition;
        std::vector<std::shared_ptr<CaseStmt>> cases;

        SwitchStmt(std::shared_ptr<Expr> condition, std::vector<std::shared_ptr<CaseStmt>> cases)
            : condition(std::move(condition)), cases(std::move(cases)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct BreakStmt : Stmt
    {
        Token keyword;

        BreakStmt(Token keyword) : keyword(std::move(keyword)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct ContinueStmt : Stmt
    {
        Token keyword;

        ContinueStmt(Token keyword) : keyword(std::move(keyword)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct FallthroughStmt : Stmt
    {
        Token keyword;

        FallthroughStmt(Token keyword) : keyword(std::move(keyword)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct StructStmt : Stmt
    {
        Token name;
        std::vector<std::shared_ptr<LetStmt>> fields;

        StructStmt(Token name, std::vector<std::shared_ptr<LetStmt>> fields)
            : name(std::move(name)), fields(std::move(fields)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct ClassStmt : Stmt
    {
        Token name;
        std::vector<std::shared_ptr<LetStmt>> fields;
        std::vector<std::shared_ptr<FunctionStmt>> methods;

        ClassStmt(Token name, std::vector<std::shared_ptr<LetStmt>> fields, std::vector<std::shared_ptr<FunctionStmt>> methods)
            : name(std::move(name)), fields(std::move(fields)), methods(std::move(methods)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

    struct EnumStmt : Stmt
    {
        Token name;
        std::vector<Token> variants;
        std::vector<std::vector<Token>> variantTypes;

        EnumStmt(Token name, std::vector<Token> variants, std::vector<std::vector<Token>> variantTypes)
            : name(std::move(name)), variants(std::move(variants)), variantTypes(std::move(variantTypes)) {}

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
    };

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
