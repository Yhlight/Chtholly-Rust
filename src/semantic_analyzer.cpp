#include "semantic_analyzer.h"
#include <stdexcept>

namespace Chtholly
{

    SemanticAnalyzer::SemanticAnalyzer()
    {
        // Primitive types have copy semantics
        copyTypes.insert("i32");
        copyTypes.insert("f64");
        copyTypes.insert("bool");
        copyTypes.insert("char");
        //copyTypes.insert("string");
    }

    void SemanticAnalyzer::analyze(const std::vector<std::shared_ptr<Stmt>>& statements)
    {
        borrowedSymbols.emplace_back();
        for (const auto& stmt : statements)
        {
            check(stmt);
        }
        borrowedSymbols.pop_back();
    }

    void SemanticAnalyzer::visit(const ReferenceExpr& expr)
    {
        if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr.expression))
        {
            SymbolInfo* info = symbolTable.lookup(varExpr->name.lexeme);
            if (!info)
            {
                throw std::runtime_error("Undeclared variable: " + varExpr->name.lexeme);
            }

            if (expr.isMutable)
            {
                if (info->sharedBorrowCount > 0 || info->mutableBorrow)
                {
                    throw std::runtime_error("Cannot mutably borrow '" + varExpr->name.lexeme + "' as it is already borrowed.");
                }
                if (!info->isMutable)
                {
                    throw std::runtime_error("Cannot mutably borrow immutable variable '" + varExpr->name.lexeme + "'.");
                }
                info->mutableBorrow = true;
            }
            else
            {
                if (info->mutableBorrow)
                {
                    throw std::runtime_error("Cannot immutably borrow '" + varExpr->name.lexeme + "' as it is already mutably borrowed.");
                }
                info->sharedBorrowCount++;
            }
            borrowedSymbols.back().push_back(varExpr->name.lexeme);
        }
        else
        {
            check(expr.expression);
        }
    }

    void SemanticAnalyzer::visit(const BinaryExpr& expr)
    {
        check(expr.left);
        check(expr.right);
    }

    void SemanticAnalyzer::visit(const UnaryExpr& expr)
    {
        check(expr.right);
    }

    void SemanticAnalyzer::visit(const LiteralExpr& expr)
    {
        // Literals are always valid
    }

    void SemanticAnalyzer::visit(const VariableExpr& expr)
    {
        SymbolInfo* info = symbolTable.lookup(expr.name.lexeme);
        if (!info)
        {
            throw std::runtime_error("Undeclared variable: " + expr.name.lexeme);
        }
        if (info->state == SymbolState::Moved)
        {
            throw std::runtime_error("Variable '" + expr.name.lexeme + "' was moved.");
        }
        if (info->mutableBorrow || info->sharedBorrowCount > 0)
        {
            throw std::runtime_error("Cannot move '" + expr.name.lexeme + "' as it is borrowed.");
        }

        // If the type is not a copy type, move it
        if (copyTypes.find(info->type) == copyTypes.end())
        {
            info->state = SymbolState::Moved;
        }

    }

    void SemanticAnalyzer::visit(const AssignExpr& expr)
    {
        SymbolInfo* info = symbolTable.lookup(expr.name.lexeme);
        if (!info)
        {
            throw std::runtime_error("Undeclared variable: " + expr.name.lexeme);
        }
        if (!info->isMutable)
        {
            throw std::runtime_error("Cannot assign to immutable variable: " + expr.name.lexeme);
        }
        if (info->state == SymbolState::Moved)
        {
            throw std::runtime_error("Cannot assign to moved variable: " + expr.name.lexeme);
        }
        if (info->sharedBorrowCount > 0)
        {
            throw std::runtime_error("Cannot assign to '" + expr.name.lexeme + "' as it is immutably borrowed.");
        }
        if (info->mutableBorrow)
        {
            throw std::runtime_error("Cannot assign to '" + expr.name.lexeme + "' as it is mutably borrowed.");
        }
        check(expr.value);

        info->state = SymbolState::Valid;
    }

    void SemanticAnalyzer::visit(const ExpressionStmt& stmt)
    {
        check(stmt.expression);
    }

    void SemanticAnalyzer::visit(const LetStmt& stmt)
    {
        if (symbolTable.isDefinedInCurrentScope(stmt.name.lexeme))
        {
            throw std::runtime_error("Variable '" + stmt.name.lexeme + "' already defined in this scope.");
        }


        if (stmt.initializer)
        {
            check(stmt.initializer);
        }

        SymbolInfo info{stmt.type.lexeme, stmt.isMutable, SymbolState::Valid};
        symbolTable.define(stmt.name.lexeme, info);
    }

    void SemanticAnalyzer::visit(const BlockStmt& stmt)
    {
        symbolTable.enterScope();
        borrowedSymbols.emplace_back();
        for (const auto& statement : stmt.statements)
        {
            check(statement);
        }
        for (const auto& name : borrowedSymbols.back())
        {
            SymbolInfo* info = symbolTable.lookup(name);
            if (info)
            {
                info->sharedBorrowCount = 0;
                info->mutableBorrow = false;
            }
        }
        borrowedSymbols.pop_back();
        symbolTable.exitScope();
    }

    void SemanticAnalyzer::visit(const IfStmt& stmt)
    {
        check(stmt.condition);
        check(stmt.thenBranch);
        if (stmt.elseBranch)
        {
            check(stmt.elseBranch);
        }
    }

    void SemanticAnalyzer::visit(const WhileStmt& stmt)
    {
        check(stmt.condition);
        check(stmt.body);
    }

    void SemanticAnalyzer::visit(const ForStmt& stmt)
    {
        symbolTable.enterScope();
        if (stmt.initializer)
        {
            check(stmt.initializer);
        }
        if (stmt.condition)
        {
            check(stmt.condition);
        }
        if (stmt.increment)
        {
            check(stmt.increment);
        }
        check(stmt.body);
        symbolTable.exitScope();
    }

    void SemanticAnalyzer::check(const std::shared_ptr<Expr>& expr)
    {
        expr->accept(*this);
    }

    void SemanticAnalyzer::check(const std::shared_ptr<Stmt>& stmt)
    {
        stmt->accept(*this);
    }

} // namespace Chtholly
