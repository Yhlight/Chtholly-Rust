#ifndef CHTHOLLY_EXPR_H
#define CHTHOLLY_EXPR_H

#include "Token.h"
#include <memory>

namespace Chtholly
{
    template <typename T>
    class ExprVisitor;

    class Expr
    {
    public:
        virtual ~Expr() = default;
        virtual std::string Accept(ExprVisitor<std::string>& visitor) = 0;
    };

    class LiteralExpr : public Expr
    {
    public:
        LiteralExpr(Token value) : value(std::move(value)) {}

        std::string Accept(ExprVisitor<std::string>& visitor) override;

        Token value;
    };

    class UnaryExpr : public Expr
    {
    public:
        UnaryExpr(Token op, std::unique_ptr<Expr> right)
            : op(std::move(op)), right(std::move(right)) {}

        std::string Accept(ExprVisitor<std::string>& visitor) override;

        Token op;
        std::unique_ptr<Expr> right;
    };

    class BinaryExpr : public Expr
    {
    public:
        BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
            : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

        std::string Accept(ExprVisitor<std::string>& visitor) override;

        std::unique_ptr<Expr> left;
        Token op;
        std::unique_ptr<Expr> right;
    };

    template <typename T>
    class ExprVisitor
    {
    public:
        virtual T Visit(LiteralExpr& expr) = 0;
        virtual T Visit(UnaryExpr& expr) = 0;
        virtual T Visit(BinaryExpr& expr) = 0;
    };

    inline std::string LiteralExpr::Accept(ExprVisitor<std::string>& visitor) { return visitor.Visit(*this); }
    inline std::string UnaryExpr::Accept(ExprVisitor<std::string>& visitor) { return visitor.Visit(*this); }
    inline std::string BinaryExpr::Accept(ExprVisitor<std::string>& visitor) { return visitor.Visit(*this); }
}

#endif //CHTHOLLY_EXPR_H
