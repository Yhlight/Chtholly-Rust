#include "SemanticAnalyzer.h"
#include <iostream>
#include <stdexcept>
#include "Type.h"

SemanticAnalyzer::SemanticAnalyzer() {
    // Pre-populate with built-in functions
    auto printlnType = std::make_shared<FunctionType>(); // This is a simplification
    symbolTable.addSymbol("println", printlnType, false);
}

void SemanticAnalyzer::analyze(BlockStmtAST& ast) {
    // First pass: register all function declarations
    for (auto& stmt : ast.statements) {
        if (auto* funcDecl = dynamic_cast<FunctionDeclAST*>(stmt.get())) {
            auto funcType = std::make_shared<FunctionType>();
            funcType->returnType = typeResolver.resolve(*funcDecl->returnType);
            for (auto& arg : funcDecl->args) {
                arg.resolvedType = typeResolver.resolve(*arg.type);
                funcType->argTypes.push_back(arg.resolvedType);
            }

            if (!symbolTable.addSymbol(funcDecl->name, funcType, false)) {
                throw std::runtime_error("Function '" + funcDecl->name + "' already declared in this scope.");
            }
        }
    }

    // Second pass: analyze all statements
    for (auto& stmt : ast.statements) {
        visit(*stmt);
    }
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ASTNode& node) {
    if (auto* p = dynamic_cast<VarDeclStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<FunctionDeclAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BlockStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BinaryExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<NumberExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<StringExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BoolExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<VariableExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<FunctionCallExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ExprStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ReturnStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<IfStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<TypeNameAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BorrowExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ReferenceTypeAST*>(&node)) {
        return visit(*p);
    }
    return nullptr;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(VarDeclStmtAST& node) {
    std::shared_ptr<Type> initType = nullptr;
    if (node.initExpr) {
        initType = visit(*node.initExpr);
        if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.initExpr.get())) {
            Symbol* symbol = symbolTable.findSymbol(varExpr->name);
            if (symbol && !symbol->type->isCopy()) {
                symbol->isMoved = true;
            }
        }
    }

    std::shared_ptr<Type> declaredType = nullptr;
    if (node.type) {
        declaredType = typeResolver.resolve(*node.type);
    }

    if (declaredType && initType && declaredType->toString() != initType->toString()) {
        throw std::runtime_error("Type mismatch for variable '" + node.varName + "'. Expected " + declaredType->toString() + " but got " + initType->toString());
    }

    auto varType = declaredType ? declaredType : initType;
    if (!varType) {
        throw std::runtime_error("Variable '" + node.varName + "' must have a type annotation or an initializer.");
    }

    if (!symbolTable.addSymbol(node.varName, varType, node.isMutable)) {
        throw std::runtime_error("Variable '" + node.varName + "' already declared in this scope.");
    }
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(FunctionDeclAST& node) {
    auto returnType = typeResolver.resolve(*node.returnType);
    currentFunction = &node;
    symbolTable.enterScope();
    for (auto& arg : node.args) {
        if(!symbolTable.addSymbol(arg.name, arg.resolvedType, false)) {
             throw std::runtime_error("Argument '" + arg.name + "' already declared in this scope.");
        }
    }
    visit(*node.body);

    if (returnType->toString() != "void") {
        bool hasReturn = false;
        if(node.body && !node.body->statements.empty()) {
            if(dynamic_cast<ReturnStmtAST*>(node.body->statements.back().get())) {
                hasReturn = true;
            }
        }

        if (!hasReturn) {
            throw std::runtime_error("Non-void function '" + node.name + "' must have a return statement as the last statement.");
        }
    }


    symbolTable.leaveScope();
    currentFunction = nullptr;
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BlockStmtAST& node) {
    symbolTable.enterScope();
    for (auto& stmt : node.statements) {
        if(stmt) visit(*stmt);
    }

    for (auto& pair : symbolTable.getCurrentScope()) {
        Symbol& symbol = pair.second;
        if (symbol.borrowedInScope) {
            symbol.immutableBorrows = 0;
            symbol.mutableBorrow = false;
            symbol.borrowedInScope = false;
        }
    }

    symbolTable.leaveScope();
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BinaryExprAST& node) {
    if (node.op == TokenType::EQUAL) {
        auto* lhsVar = dynamic_cast<VariableExprAST*>(node.lhs.get());
        if (!lhsVar) {
            throw std::runtime_error("The left-hand side of an assignment must be a variable.");
        }

        Symbol* lhsSymbol = symbolTable.findSymbol(lhsVar->name);
        if (!lhsSymbol) {
            throw std::runtime_error("Variable '" + lhsVar->name + "' not declared.");
        }
        if (!lhsSymbol->isMutable) {
            throw std::runtime_error("Variable '" + lhsVar->name + "' is not mutable.");
        }

        auto rhsType = visit(*node.rhs);
        if (auto* rhsVar = dynamic_cast<VariableExprAST*>(node.rhs.get())) {
            Symbol* rhsSymbol = symbolTable.findSymbol(rhsVar->name);
            if (rhsSymbol && !rhsSymbol->type->isCopy()) {
                rhsSymbol->isMoved = true;
            }
        }

        if (lhsSymbol->type->toString() != rhsType->toString()) {
            throw std::runtime_error("Type mismatch in assignment to variable '" + lhsVar->name + "'.");
        }
        node.type = lhsSymbol->type;
        return node.type;
    }


    auto lhsType = visit(*node.lhs);
    auto rhsType = visit(*node.rhs);

    if (!lhsType || !rhsType) {
        throw std::runtime_error("Could not resolve type for one or both operands in binary expression.");
    }

    if (node.op == TokenType::LESS || node.op == TokenType::LESS_EQUAL || node.op == TokenType::GREATER || node.op == TokenType::GREATER_EQUAL || node.op == TokenType::DOUBLE_EQUAL || node.op == TokenType::NOT_EQUAL) {
        if (lhsType->toString() != rhsType->toString()) {
            throw std::runtime_error("Type mismatch in binary expression: " + lhsType->toString() + " vs " + rhsType->toString());
        }
        node.type = std::make_shared<BoolType>();
        return node.type;
    }

    if ((!lhsType->isInteger() && !lhsType->isFloat()) || (!rhsType->isInteger() && !rhsType->isFloat())) {
        throw std::runtime_error("Binary operator applied to non-numeric type.");
    }

    if (lhsType->toString() != rhsType->toString()) {
        throw std::runtime_error("Type mismatch in binary expression: " + lhsType->toString() + " vs " + rhsType->toString());
    }

    node.type = lhsType;
    return lhsType;
}

#include <cmath>

std::shared_ptr<Type> SemanticAnalyzer::visit(NumberExprAST& node) {
    double intpart;
    if (modf(node.value, &intpart) == 0.0) {
        node.type = std::make_shared<IntegerType>(32, true);
    } else {
        node.type = std::make_shared<FloatType>(64);
    }
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(StringExprAST& node) {
    node.type = std::make_shared<StringType>();
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BoolExprAST& node) {
    node.type = std::make_shared<BoolType>();
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(VariableExprAST& node) {
    Symbol* symbol = symbolTable.findSymbol(node.name);
    if (!symbol) {
        throw std::runtime_error("Variable '" + node.name + "' not declared.");
    }
    if (symbol->isMoved) {
        throw std::runtime_error("Variable '" + node.name + "' has been moved and is no longer valid.");
    }
    node.type = symbol->type;
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(FunctionCallExprAST& node) {
    if (node.callee == "println") {
        for (auto& arg : node.args) {
            visit(*arg);
            if (auto* varExpr = dynamic_cast<VariableExprAST*>(arg.get())) {
                Symbol* argSymbol = symbolTable.findSymbol(varExpr->name);
                if (argSymbol && argSymbol->isMoved) {
                    throw std::runtime_error("Variable '" + varExpr->name + "' has been moved and is no longer valid.");
                }
                // Do not move the argument for println
            }
        }
        return std::make_shared<VoidType>();
    }

    Symbol* symbol = symbolTable.findSymbol(node.callee);
    if (!symbol) {
        throw std::runtime_error("Function '" + node.callee + "' not declared.");
    }

    // In a real implementation, we would check argument types and arity.
    for (auto& arg : node.args) {
        visit(*arg);
        if (auto* varExpr = dynamic_cast<VariableExprAST*>(arg.get())) {
            Symbol* argSymbol = symbolTable.findSymbol(varExpr->name);
            if (argSymbol && !argSymbol->type->isCopy()) {
                argSymbol->isMoved = true;
            }
        }
    }

    // For now, we'll just assume the call is valid and return the function's return type.
    if (auto* funcType = dynamic_cast<FunctionType*>(symbol->type.get())) {
        node.type = funcType->returnType;
        return node.type;
    }
    throw std::runtime_error("Symbol '" + node.callee + "' is not a function, cannot be called");
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ExprStmtAST& node) {
    visit(*node.expr);
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ReturnStmtAST& node) {
    if (!currentFunction) {
        throw std::runtime_error("Return statement outside of a function.");
    }
    if (node.returnValue) {
        auto returnType = visit(*node.returnValue);
        auto expectedType = typeResolver.resolve(*currentFunction->returnType);
        if (returnType->toString() != expectedType->toString()) {
            throw std::runtime_error("Return type mismatch: expected " + expectedType->toString() + ", got " + returnType->toString());
        }
    } else {
        auto expectedType = typeResolver.resolve(*currentFunction->returnType);
        if (expectedType->toString() != "void") {
            throw std::runtime_error("Non-void function must return a value.");
        }
    }
    return nullptr;
}


std::shared_ptr<Type> SemanticAnalyzer::visit(IfStmtAST& node) {
    auto conditionType = visit(*node.condition);
    if (!conditionType->isBool()) {
        throw std::runtime_error("If condition must be a boolean expression.");
    }

    visit(*node.thenBranch);
    if (node.elseBranch) {
        visit(*node.elseBranch);
    }

    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(TypeNameAST& node) {
    return nullptr;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BorrowExprAST& node) {
    auto exprType = visit(*node.expression);
    if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.expression.get())) {
        Symbol* symbol = symbolTable.findSymbol(varExpr->name);
        if (node.isMutable) {
            if (!symbol->isMutable) {
                throw std::runtime_error("Cannot mutably borrow immutable variable '" + varExpr->name + "'.");
            }
            if (symbol->mutableBorrow || symbol->immutableBorrows > 0) {
                throw std::runtime_error("Cannot mutably borrow '" + varExpr->name + "' as it is already borrowed.");
            }
            symbol->mutableBorrow = true;
            symbol->borrowedInScope = true;
        } else {
            if (symbol->mutableBorrow) {
                throw std::runtime_error("Cannot immutably borrow '" + varExpr->name + "' as it is already mutably borrowed.");
            }
            symbol->immutableBorrows++;
            symbol->borrowedInScope = true;
        }
    }
    node.type = std::make_shared<ReferenceType>(exprType, node.isMutable);
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ReferenceTypeAST& node) {
    auto referencedType = typeResolver.resolve(*node.referencedType);
    return std::make_shared<ReferenceType>(referencedType, node.isMutable);
}
