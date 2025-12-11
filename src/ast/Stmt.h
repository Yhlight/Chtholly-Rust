#ifndef CHTHOLLY_STMT_H
#define CHTHOLLY_STMT_H

#include "Expr.h"
#include "Token.h"
#include <memory>
#include <vector>

namespace Chtholly
{
    template <typename T>
    class StmtVisitor;

    class Stmt
    {
    public:
        virtual ~Stmt() = default;
        virtual std::string Accept(StmtVisitor<std::string>& visitor) = 0;
    };

    class LetDeclStmt : public Stmt
    {
    public:
        LetDeclStmt(Token name, std::unique_ptr<Expr> initializer)
            : name(std::move(name)), initializer(std::move(initializer)) {}

        std::string Accept(StmtVisitor<std::string>& visitor) override;

        Token name;
        std::unique_ptr<Expr> initializer;
    };

    class ExpressionStmt : public Stmt
    {
    public:
        ExpressionStmt(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {}

        std::string Accept(StmtVisitor<std::string>& visitor) override;

        std::unique_ptr<Expr> expression;
    };

    template <typename T>
    class StmtVisitor
    {
    public:
        virtual T Visit(LetDeclStmt& stmt) = 0;
        virtual T Visit(ExpressionStmt& stmt) = 0;
    };

    inline std::string LetDeclStmt::Accept(StmtVisitor<std::string>& visitor) { return visitor.Visit(*this); }
    inline std::string ExpressionStmt::Accept(StmtVisitor<std::string>& visitor) { return visitor.Visit(*this); }
}

#endif //CHTHOLLY_STMT_H
