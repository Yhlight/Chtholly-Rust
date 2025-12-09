#include "sema.h"
#include <iostream>

namespace Chtholly
{

    // SymbolTable implementation
    SymbolTable::SymbolTable() : currentScopeLevel(0)
    {
        enterScope(); // Global scope
    }

    void SymbolTable::enterScope()
    {
        scopes.emplace_back();
        currentScopeLevel++;
    }

    void SymbolTable::exitScope()
    {
        scopes.pop_back();
        currentScopeLevel--;
    }

    bool SymbolTable::define(const std::string& name, const std::string& type, bool isMutable)
    {
        if (scopes.back().count(name))
        {
            return false; // Already defined in this scope
        }
        auto symbol = std::make_shared<Symbol>();
        symbol->name = name;
        symbol->type = type;
        symbol->isMutable = isMutable;
        symbol->state = SymbolState::DECLARED;
        symbol->scopeLevel = currentScopeLevel;
        scopes.back()[name] = symbol;
        return true;
    }

    std::shared_ptr<Symbol> SymbolTable::lookup(const std::string& name)
    {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            if (it->count(name))
            {
                return (*it)[name];
            }
        }
        return nullptr;
    }

    void SymbolTable::setState(const std::string& name, SymbolState state)
    {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            if (it->count(name))
            {
                (*it)[name]->state = state;
                return;
            }
        }
    }


    // Sema implementation
    Sema::Sema() {}

    void Sema::analyze(const std::vector<std::shared_ptr<Stmt>>& statements)
    {
        for (const auto& stmt : statements)
        {
            analyze(stmt);
        }
    }

    void Sema::analyze(const std::shared_ptr<Stmt>& stmt)
    {
        stmt->accept(*this);
    }

    void Sema::analyze(const std::shared_ptr<Expr>& expr)
    {
        expr->accept(*this);
    }

    void Sema::visit(const ExpressionStmt& stmt)
    {
        analyze(stmt.expression);
    }

    void Sema::visit(const LetStmt& stmt)
    {
        if (!symbolTable.define(stmt.name.lexeme, stmt.type.lexeme, stmt.isMutable))
        {
            std::cerr << "Error: Variable '" << stmt.name.lexeme << "' already defined in this scope." << std::endl;
        }

        if (stmt.initializer)
        {
            analyze(stmt.initializer);
            symbolTable.setState(stmt.name.lexeme, SymbolState::INITIALIZED);

            if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(stmt.initializer))
            {
                auto rhsSymbol = symbolTable.lookup(varExpr->name.lexeme);
                if (rhsSymbol)
                {
                    symbolTable.setState(rhsSymbol->name, SymbolState::MOVED);
                }
            }
        }
    }

    void Sema::visit(const BlockStmt& stmt)
    {
        symbolTable.enterScope();
        for (const auto& statement : stmt.statements)
        {
            analyze(statement);
        }
        symbolTable.exitScope();
    }

    void Sema::visit(const IfStmt& stmt)
    {
        analyze(stmt.condition);
        analyze(stmt.thenBranch);
        if (stmt.elseBranch)
        {
            analyze(stmt.elseBranch);
        }
    }

    void Sema::visit(const WhileStmt& stmt)
    {
        analyze(stmt.condition);
        analyze(stmt.body);
    }

    void Sema::visit(const ForStmt& stmt)
    {
        symbolTable.enterScope();
        if(stmt.initializer)
        {
            analyze(stmt.initializer);
        }
        if(stmt.condition)
        {
            analyze(stmt.condition);
        }
        if(stmt.increment)
        {
            analyze(stmt.increment);
        }
        analyze(stmt.body);
        symbolTable.exitScope();
    }

    void Sema::visit(const BinaryExpr& expr)
    {
        analyze(expr.left);
        analyze(expr.right);
    }

    void Sema::visit(const UnaryExpr& expr)
    {
        analyze(expr.right);
    }

    void Sema::visit(const LiteralExpr& expr)
    {
        // Literals don't need semantic analysis
    }

    void Sema::visit(const VariableExpr& expr)
    {
        auto symbol = symbolTable.lookup(expr.name.lexeme);
        if (!symbol)
        {
            std::cerr << "Error: Undefined variable '" << expr.name.lexeme << "'." << std::endl;
            return;
        }

        if (symbol->state == SymbolState::MOVED)
        {
            std::cerr << "Error: Use of moved value '" << expr.name.lexeme << "'." << std::endl;
        }
    }

    void Sema::visit(const AssignExpr& expr)
    {
        auto symbol = symbolTable.lookup(expr.name.lexeme);
        if (!symbol)
        {
            std::cerr << "Error: Undefined variable '" << expr.name.lexeme << "'." << std::endl;
            return;
        }

        if (!symbol->isMutable && symbol->state == SymbolState::INITIALIZED)
        {
            std::cerr << "Error: Cannot assign to immutable variable '" << expr.name.lexeme << "'." << std::endl;
        }

        analyze(expr.value);

        if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr.value))
        {
            auto rhsSymbol = symbolTable.lookup(varExpr->name.lexeme);
            if (rhsSymbol)
            {
                symbolTable.setState(rhsSymbol->name, SymbolState::MOVED);
            }
        }

        symbolTable.setState(expr.name.lexeme, SymbolState::INITIALIZED);
    }

} // namespace Chtholly
