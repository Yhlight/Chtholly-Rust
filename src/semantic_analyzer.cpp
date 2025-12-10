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
                }
                else
                {
                    if (info->mutableBorrow)
                    {
                        throw std::runtime_error("Cannot immutably borrow '" + varExpr->name.lexeme + "' as it is already mutably borrowed.");
                    }
                    info->sharedBorrowCount++;
                }
                symbolTable.borrow(varExpr->name.lexeme);
            }
        }
        else
        {
            check(expr.right);
        }
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

        checkForDanglingReference(info, expr.value);

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

        std::string varType = stmt.type.lexeme;
        if (stmt.initializer)
        {
            check(stmt.initializer);
            if (varType.empty())
            {
                varType = typeOf(stmt.initializer);
            }
        }

        SymbolInfo info(varType, stmt.isMutable, SymbolState::Valid);
        symbolTable.define(stmt.name.lexeme, info);

        if (stmt.initializer)
        {
            SymbolInfo* lhsInfo = symbolTable.lookup(stmt.name.lexeme);
            checkForDanglingReference(lhsInfo, stmt.initializer);
        }
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
        LoopType enclosingLoop = currentLoop;
        currentLoop = LoopType::LOOP;
        check(stmt.body);
        currentLoop = enclosingLoop;
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
        LoopType enclosingLoop = currentLoop;
        currentLoop = LoopType::LOOP;
        check(stmt.body);
        currentLoop = enclosingLoop;
        symbolTable.exitScope();
    }

    void SemanticAnalyzer::visit(const SwitchStmt& stmt)
    {
        check(stmt.condition);
        std::string switchType = typeOf(stmt.condition);

        bool hasDefault = false;
        std::set<int> intCaseValues;
        std::set<std::string> stringCaseValues;

        LoopType enclosingLoop = currentLoop;
        currentLoop = LoopType::SWITCH;
        for (const auto& caseStmt : stmt.cases)
        {
            if (caseStmt->condition)
            {
                check(caseStmt->condition);
                std::string caseType = typeOf(caseStmt->condition);
                if (caseType != switchType)
                {
                    throw std::runtime_error("Case type does not match switch type.");
                }

                if (auto literalExpr = std::dynamic_pointer_cast<LiteralExpr>(caseStmt->condition))
                {
                    if (std::holds_alternative<int>(literalExpr->value))
                    {
                        int value = std::get<int>(literalExpr->value);
                        if (intCaseValues.count(value))
                        {
                            throw std::runtime_error("Duplicate case value.");
                        }
                        intCaseValues.insert(value);
                    }
                    else if (std::holds_alternative<std::string>(literalExpr->value))
                    {
                        std::string value = std::get<std::string>(literalExpr->value);
                        if (stringCaseValues.count(value))
                        {
                            throw std::runtime_error("Duplicate case value.");
                        }
                        stringCaseValues.insert(value);
                    }
                }
            }
            else
            {
                if (hasDefault)
                {
                    throw std::runtime_error("Multiple default cases in switch statement.");
                }
                hasDefault = true;
            }

            check(caseStmt);
        }
        currentLoop = enclosingLoop;
    }

    void SemanticAnalyzer::visit(const CaseStmt& stmt)
    {
        if (stmt.condition)
        {
            check(stmt.condition);
        }

        if (auto block = std::dynamic_pointer_cast<BlockStmt>(stmt.body))
        {
            for (size_t i = 0; i < block->statements.size(); ++i)
            {
                if (std::dynamic_pointer_cast<FallthroughStmt>(block->statements[i]))
                {
                    if (i != block->statements.size() - 1)
                    {
                        throw std::runtime_error("Fallthrough must be the last statement in a case block.");
                    }
                }
                check(block->statements[i]);
            }
        }
        else
        {
            check(stmt.body);
        }
    }

    void SemanticAnalyzer::visit(const BreakStmt& stmt)
    {
        if (currentLoop == LoopType::NONE)
        {
            throw std::runtime_error("Cannot break from outside a loop or switch.");
        }
    }

    void SemanticAnalyzer::visit(const ContinueStmt& stmt)
    {
        if (currentLoop != LoopType::LOOP)
        {
            throw std::runtime_error("Cannot continue from outside a loop.");
        }
    }

    void SemanticAnalyzer::visit(const FallthroughStmt& stmt)
    {
        if (currentLoop != LoopType::SWITCH)
        {
            throw std::runtime_error("Cannot fallthrough from outside a switch.");
        }
    }

    void SemanticAnalyzer::check(const std::shared_ptr<Expr>& expr)
    {
        expr->accept(*this);
    }

    void SemanticAnalyzer::check(const std::shared_ptr<Stmt>& stmt)
    {
        stmt->accept(*this);
    }

    void SemanticAnalyzer::checkForDanglingReference(const SymbolInfo* lhsInfo, const std::shared_ptr<Expr>& rhsExpr)
    {
        if (auto unaryExpr = std::dynamic_pointer_cast<UnaryExpr>(rhsExpr))
        {
            if (unaryExpr->op.type == TokenType::AMPERSAND)
            {
                if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(unaryExpr->right))
                {
                    SymbolInfo* rhsInfo = symbolTable.lookup(varExpr->name.lexeme);
                    if (rhsInfo && lhsInfo->lifetime < rhsInfo->lifetime)
                    {
                        throw std::runtime_error("Dangling reference: variable '" + varExpr->name.lexeme + "' does not live long enough.");
                    }
                }
            }
        }
    }

    void SemanticAnalyzer::visit(const FunctionStmt& stmt)
    {
        SymbolInfo funcInfo;
        funcInfo.symbolType = SymbolType::Function;
        funcInfo.returnType = stmt.returnType.lexeme;
        for (const auto& paramType : stmt.parameterTypes)
        {
            funcInfo.parameterTypes.push_back(paramType.lexeme);
        }
        symbolTable.define(stmt.name.lexeme, funcInfo);

        FunctionType enclosingFunction = currentFunction;
        currentFunction = FunctionType::FUNCTION;
        std::string enclosingReturnType = currentReturnType;
        currentReturnType = funcInfo.returnType;

        symbolTable.enterScope();
        for (size_t i = 0; i < stmt.parameters.size(); ++i)
        {
            SymbolInfo paramInfo{stmt.parameterTypes[i].lexeme, false, SymbolState::Valid};
            symbolTable.define(stmt.parameters[i].lexeme, paramInfo);
        }

        for (const auto& statement : stmt.body->statements)
        {
            check(statement);
        }
        symbolTable.exitScope();
        currentFunction = enclosingFunction;
        currentReturnType = enclosingReturnType;
    }

    void SemanticAnalyzer::visit(const ReturnStmt& stmt)
    {
        if (currentFunction == FunctionType::NONE)
        {
            throw std::runtime_error("Cannot return from top-level code.");
        }

        if (stmt.value)
        {
            std::string valueType = typeOf(stmt.value);
            if (valueType != currentReturnType)
            {
                throw std::runtime_error("Return type mismatch. Expected " + currentReturnType + " but got " + valueType + ".");
            }

            if (auto unaryExpr = std::dynamic_pointer_cast<UnaryExpr>(stmt.value))
            {
                if (unaryExpr->op.type == TokenType::AMPERSAND)
                {
                    if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(unaryExpr->right))
                    {
                        SymbolInfo* info = symbolTable.lookup(varExpr->name.lexeme);
                        if (info && info->lifetime == symbolTable.getCurrentLifetime())
                        {
                            throw std::runtime_error("Cannot return a reference to a local variable.");
                        }
                    }
                }
            }
        }
        else if (currentReturnType != "void")
        {
            throw std::runtime_error("Return type mismatch. Expected " + currentReturnType + " but got void.");
        }
    }

    void SemanticAnalyzer::visit(const CallExpr& expr)
    {
        check(expr.callee);

        for (const auto& arg : expr.arguments)
        {
            check(arg);
        }

        if (auto getExpr = std::dynamic_pointer_cast<GetExpr>(expr.callee))
        {
            std::string objectType = typeOf(getExpr->object);
            SymbolInfo* classInfo = symbolTable.lookup(objectType);
            if (classInfo && classInfo->symbolType == SymbolType::Class)
            {
                auto methodIt = classInfo->fields.find(getExpr->name.lexeme);
                if (methodIt != classInfo->fields.end())
                {
                    SymbolInfo& methodInfo = methodIt->second;
                    if (methodInfo.symbolType == SymbolType::Function)
                    {
                        if (expr.arguments.size() != methodInfo.parameterTypes.size())
                        {
                            throw std::runtime_error("Expected " + std::to_string(methodInfo.parameterTypes.size()) + " arguments but got " + std::to_string(expr.arguments.size()) + ".");
                        }

                        for (size_t i = 0; i < expr.arguments.size(); ++i)
                        {
                            std::string argType = typeOf(expr.arguments[i]);
                            if (argType != methodInfo.parameterTypes[i])
                            {
                                throw std::runtime_error("Argument type mismatch for parameter " + std::to_string(i + 1) + ". Expected " + methodInfo.parameterTypes[i] + " but got " + argType + ".");
                            }
                        }
                    }
                }
            }
        }
        else if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr.callee))
        {
            SymbolInfo* info = symbolTable.lookup(varExpr->name.lexeme);
            if (!info || (info->symbolType != SymbolType::Function && info->symbolType != SymbolType::Class))
            {
                throw std::runtime_error("Can only call functions and classes.");
            }

            if (info->symbolType == SymbolType::Function && expr.arguments.size() != info->parameterTypes.size())
            {
                throw std::runtime_error("Expected " + std::to_string(info->parameterTypes.size()) + " arguments but got " + std::to_string(expr.arguments.size()) + ".");
            }

            for (size_t i = 0; i < expr.arguments.size(); ++i)
            {
                std::string argType = typeOf(expr.arguments[i]);
                if (argType != info->parameterTypes[i])
                {
                    throw std::runtime_error("Argument type mismatch for parameter " + std::to_string(i + 1) + ". Expected " + info->parameterTypes[i] + " but got " + argType + ".");
                }
            }
        }
    }

    std::string SemanticAnalyzer::typeOf(const std::shared_ptr<Expr>& expr)
    {
        if (auto literalExpr = std::dynamic_pointer_cast<LiteralExpr>(expr))
        {
            return std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) return "i32";
                if constexpr (std::is_same_v<T, double>) return "f64";
                if constexpr (std::is_same_v<T, std::string>) return "string";
                if constexpr (std::is_same_v<T, char>) return "char";
                if constexpr (std::is_same_v<T, bool>) return "bool";
                return "unknown";
            }, literalExpr->value);
        }

        if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr))
        {
            SymbolInfo* info = symbolTable.lookup(varExpr->name.lexeme);
            if (info)
            {
                return info->type;
            }
        }

        if (auto binaryExpr = std::dynamic_pointer_cast<BinaryExpr>(expr))
        {
            switch (binaryExpr->op.type)
            {
                case TokenType::GREATER:
                case TokenType::GREATER_EQUAL:
                case TokenType::LESS:
                case TokenType::LESS_EQUAL:
                case TokenType::BANG_EQUAL:
                case TokenType::EQUAL_EQUAL:
                    return "bool";
                default:
                    return typeOf(binaryExpr->left);
            }
        }

        if (auto unaryExpr = std::dynamic_pointer_cast<UnaryExpr>(expr))
        {
            if (unaryExpr->op.type == TokenType::AMPERSAND)
            {
                return "&" + typeOf(unaryExpr->right);
            }
        }

        if (auto callExpr = std::dynamic_pointer_cast<CallExpr>(expr))
        {
            if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(callExpr->callee))
            {
                SymbolInfo* info = symbolTable.lookup(varExpr->name.lexeme);
                if (info && info->symbolType == SymbolType::Function)
                {
                    return info->returnType;
                }
            }
        }
        if (auto getExpr = std::dynamic_pointer_cast<GetExpr>(expr))
        {
            std::string objectType = typeOf(getExpr->object);
            SymbolInfo* classInfo = symbolTable.lookup(objectType);
            if (classInfo && (classInfo->symbolType == SymbolType::Struct || classInfo->symbolType == SymbolType::Class))
            {
                auto fieldIt = classInfo->fields.find(getExpr->name.lexeme);
                if (fieldIt != classInfo->fields.end())
                {
                    if (fieldIt->second.symbolType == SymbolType::Function)
                    {
                        return fieldIt->second.returnType;
                    }
                    return fieldIt->second.type;
                }
            }
        }
        if (auto structInitializerExpr = std::dynamic_pointer_cast<StructInitializerExpr>(expr))
        {
            return structInitializerExpr->name.lexeme;
        }

        if (auto thisExpr = std::dynamic_pointer_cast<ThisExpr>(expr))
        {
            SymbolInfo* info = symbolTable.lookup(thisExpr->keyword.lexeme);
            if (info)
            {
                return info->type;
            }
        }

        return "unknown";
    }

    void SemanticAnalyzer::visit(const StructStmt& stmt)
    {
        SymbolInfo info{stmt.name.lexeme, false, SymbolState::Valid, SymbolType::Struct};
        for (const auto& field : stmt.fields)
        {
            SymbolInfo fieldInfo{field->type.lexeme, field->isMutable, SymbolState::Valid};
            info.fields[field->name.lexeme] = fieldInfo;
        }
        symbolTable.define(stmt.name.lexeme, info);
    }

    void SemanticAnalyzer::visit(const GetExpr& expr)
    {
        check(expr.object);
        std::string objectType = typeOf(expr.object);
        SymbolInfo* classInfo = symbolTable.lookup(objectType);
        if (!classInfo || (classInfo->symbolType != SymbolType::Struct && classInfo->symbolType != SymbolType::Class))
        {
            throw std::runtime_error("Cannot access property on non-struct or non-class type.");
        }

        auto fieldIt = classInfo->fields.find(expr.name.lexeme);
        if (fieldIt == classInfo->fields.end())
        {
            // It's not a field, so it must be a method.
            // We don't have to do anything here, because the typeOf method will handle it.
        }
    }

    void SemanticAnalyzer::visit(const SetExpr& expr)
    {
        check(expr.value);
        check(expr.object);

        std::string objectType = typeOf(expr.object);
        SymbolInfo* classInfo = symbolTable.lookup(objectType);
        if (!classInfo || (classInfo->symbolType != SymbolType::Struct && classInfo->symbolType != SymbolType::Class))
        {
            throw std::runtime_error("Cannot access property on non-struct or non-class type.");
        }

        auto fieldIt = classInfo->fields.find(expr.name.lexeme);
        if (fieldIt == classInfo->fields.end())
        {
            throw std::runtime_error("Class " + objectType + " has no field named " + expr.name.lexeme + ".");
        }

        if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr.object)) {
            SymbolInfo* objectInfo = symbolTable.lookup(varExpr->name.lexeme);
            if (objectInfo && !objectInfo->isMutable) {
                throw std::runtime_error("Cannot assign to field of immutable object.");
            }
        }

        if (!fieldIt->second.isMutable)
        {
            throw std::runtime_error("Cannot assign to immutable field.");
        }
    }

    void SemanticAnalyzer::visit(const StructInitializerExpr& expr)
    {
        SymbolInfo* info = symbolTable.lookup(expr.name.lexeme);
        if (!info || info->symbolType != SymbolType::Struct)
        {
            throw std::runtime_error("Not a struct type: " + expr.name.lexeme);
        }

        for (const auto& initializer : expr.initializers)
        {
            check(initializer.second);
        }
    }

    void SemanticAnalyzer::visit(const ClassStmt& stmt)
    {
        SymbolInfo classInfo(stmt.name.lexeme, false, SymbolState::Valid, SymbolType::Class);
        for (const auto& field : stmt.fields)
        {
            SymbolInfo fieldInfo(field->type.lexeme, field->isMutable, SymbolState::Valid);
            classInfo.fields[field->name.lexeme] = fieldInfo;
        }
        for (const auto& method : stmt.methods)
        {
            SymbolInfo methodInfo(method->returnType.lexeme, false, SymbolState::Valid, SymbolType::Function);
            for (const auto& paramType : method->parameterTypes)
            {
                methodInfo.parameterTypes.push_back(paramType.lexeme);
            }
            classInfo.fields[method->name.lexeme] = methodInfo;
        }
        symbolTable.define(stmt.name.lexeme, classInfo);

        ClassType enclosingClass = currentClass;
        currentClass = ClassType::CLASS;
        symbolTable.enterScope();
        SymbolInfo selfInfo(stmt.name.lexeme, false, SymbolState::Valid);
        symbolTable.define("self", selfInfo);
        for (const auto& method : stmt.methods)
        {
            check(method);
        }
        symbolTable.exitScope();
        currentClass = enclosingClass;
    }

    void SemanticAnalyzer::visit(const ThisExpr& expr)
    {
        if (currentClass == ClassType::NONE)
        {
            throw std::runtime_error("Cannot use 'self' outside of a class.");
        }
        check(std::make_shared<VariableExpr>(expr.keyword));
    }

} // namespace Chtholly
