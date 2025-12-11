#ifndef CHTHOLLY_EXPR_H
#define CHTHOLLY_EXPR_H

#include "../Token.h"
#include <any>
#include <memory>
#include <vector>

namespace Chtholly
{

// Forward declarations
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct ExprVisitor; // Forward declare visitor

// Base class for all expression nodes
struct Expr
{
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

// Visitor interface
struct ExprVisitor
{
    virtual std::any visit(const Binary& expr) = 0;
    virtual std::any visit(const Grouping& expr) = 0;
    virtual std::any visit(const Literal& expr) = 0;
    virtual std::any visit(const Unary& expr) = 0;
    virtual ~ExprVisitor() = default;
};


// Derived classes for each expression type
struct Binary final : Expr
{
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    {
    }

    std::any accept(ExprVisitor& visitor) const override
    {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

struct Grouping final : Expr
{
    Grouping(std::unique_ptr<Expr> expression)
        : expression(std::move(expression))
    {
    }

    std::any accept(ExprVisitor& visitor) const override
    {
        return visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct Literal final : Expr
{
    Literal(std::any value)
        : value(std::move(value))
    {
    }

    std::any accept(ExprVisitor& visitor) const override
    {
        return visitor.visit(*this);
    }

    const std::any value;
};

struct Unary final : Expr
{
    Unary(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right))
    {
    }

    std::any accept(ExprVisitor& visitor) const override
    {
        return visitor.visit(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};


} // namespace Chtholly

#endif // CHTHOLLY_EXPR_H
