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
        for (const auto& stmt : statements)
        {
            check(stmt);
        }
    }

    void SemanticAnalyzer::visit(const BinaryExpr& expr)
    {
        check(expr.left);
        check(expr.right);
        if (type(expr.left) != type(expr.right))
        {
            throw std::runtime_error("Mismatched types in binary expression.");
        }
        expr.type = type(expr.left);
    }

    void SemanticAnalyzer::visit(const UnaryExpr& expr)
    {
        if (expr.op.type == TokenType::AMPERSAND)
        {
            if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr.right))
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
                    expr.type = "&mut " + info->type;
                }
                else
                {
                    if (info->mutableBorrow)
                    {
                        throw std::runtime_error("Cannot immutably borrow '" + varExpr->name.lexeme + "' as it is already mutably borrowed.");
                    }
                    info->sharedBorrowCount++;
                    expr.type = "&" + info->type;
                }
                symbolTable.borrow(varExpr->name.lexeme);
            }
        }
        else
        {
            check(expr.right);
            expr.type = type(expr.right);
        }
    }

    void SemanticAnalyzer::visit(const LiteralExpr& expr)
    {
        if (std::holds_alternative<int>(expr.value))
        {
            expr.type = "i32";
        }
        else if (std::holds_alternative<double>(expr.value))
        {
            expr.type = "f64";
        }
        else if (std::holds_alternative<std::string>(expr.value))
        {
            expr.type = "string";
        }
        else if (std::holds_alternative<char>(expr.value))
        {
            expr.type = "char";
        }
        else if (std::holds_alternative<bool>(expr.value))
        {
            expr.type = "bool";
        }
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

        expr.type = info->type;

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
        if (info->sharedBorrowCount > 0)
        {
            throw std::runtime_error("Cannot assign to '" + expr.name.lexeme + "' as it is immutably borrowed.");
        }
        if (info->mutableBorrow)
        {
            throw std::runtime_error("Cannot assign to '" + expr.name.lexeme + "' as it is mutably borrowed.");
        }
        check(expr.value);
        if (type(expr.value) != info->type)
        {
            throw std::runtime_error("Mismatched types in assignment.");
        }

        info->state = SymbolState::Valid;
        expr.type = info->type;
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

        std::string varType;
        if (!stmt.type.lexeme.empty())
        {
            varType = stmt.type.lexeme;
        }

        if (stmt.initializer)
        {
            check(stmt.initializer);
            if (varType.empty())
            {
                varType = type(stmt.initializer);
            }
            else
            {
                std::string initializerType = type(stmt.initializer);
                if (varType != initializerType)
                {
                    throw std::runtime_error("Mismatched types in variable declaration.");
                }
            }
        }

        if (varType.empty())
        {
            throw std::runtime_error("Cannot infer type of variable '" + stmt.name.lexeme + "'.");
        }

        SymbolInfo info{varType, stmt.isMutable, SymbolState::Valid};
        symbolTable.define(stmt.name.lexeme, info);
    }

    void SemanticAnalyzer::visit(const BlockStmt& stmt)
    {
        symbolTable.enterScope();
        for (const auto& statement : stmt.statements)
        {
            check(statement);
        }
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

    std::string SemanticAnalyzer::type(const std::shared_ptr<Expr>& expr)
    {
        return expr->type;
    }

    void SemanticAnalyzer::visit(const FunctionStmt& stmt)
    {
        SymbolInfo info;
        info.type = "function";
        info.returnType = stmt.returnType.lexeme;
        for (const auto& param : stmt.params)
        {
            info.paramTypes.push_back(param.type.lexeme);
        }
        symbolTable.define(stmt.name.lexeme, info);

        symbolTable.enterScope();
        FunctionInfo functionInfo{stmt.returnType.lexeme};
        FunctionInfo* oldFunction = currentFunction;
        currentFunction = &functionInfo;

        for (const auto& param : stmt.params)
        {
            SymbolInfo paramInfo{param.type.lexeme, false, SymbolState::Valid};
            symbolTable.define(param.name.lexeme, paramInfo);
        }

        check(stmt.body);

        currentFunction = oldFunction;
        symbolTable.exitScope();
    }

    void SemanticAnalyzer::visit(const CallExpr& expr)
    {
        check(expr.callee);

        SymbolInfo* info = nullptr;
        if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr.callee))
        {
            info = symbolTable.lookup(varExpr->name.lexeme);
        }

        if (!info || info->type != "function")
        {
            throw std::runtime_error("Can only call functions.");
        }

        if (expr.arguments.size() != info->paramTypes.size())
        {
            throw std::runtime_error("Expected " + std::to_string(info->paramTypes.size()) +
                                     " arguments but got " + std::to_string(expr.arguments.size()) + ".");
        }

        for (size_t i = 0; i < expr.arguments.size(); ++i)
        {
            check(expr.arguments[i]);
            if (type(expr.arguments[i]) != info->paramTypes[i])
            {
                throw std::runtime_error("Mismatched types in function call.");
            }
        }

        expr.type = info->returnType;
    }

    void SemanticAnalyzer::visit(const ReturnStmt& stmt)
    {
        if (!currentFunction)
        {
            throw std::runtime_error("Cannot return from top-level code.");
        }

        if (stmt.value)
        {
            if (currentFunction->returnType.empty())
            {
                throw std::runtime_error("Cannot return a value from a function with no return type.");
            }
            check(stmt.value);
        }
        else
        {
            if (!currentFunction->returnType.empty() && currentFunction->returnType != "void")
            {
                throw std::runtime_error("Cannot return without a value from a function with a return type.");
            }
        }
    }

} // namespace Chtholly
