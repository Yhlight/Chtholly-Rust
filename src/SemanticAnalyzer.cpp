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
    visit(ast);
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
    } else if (auto* p = dynamic_cast<VariableExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<FunctionCallExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ExprStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ReturnStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<TypeNameAST*>(&node)) {
        return visit(*p);
    }
    return nullptr;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(VarDeclStmtAST& node) {
    std::shared_ptr<Type> initType = nullptr;
    if (node.initExpr) {
        initType = visit(*node.initExpr);
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
    if (!symbolTable.addSymbol(node.name, returnType, false)) {
        throw std::runtime_error("Function '" + node.name + "' already declared in this scope.");
    }

    currentFunction = &node;
    symbolTable.enterScope();
    for (auto& arg : node.args) {
        auto argType = typeResolver.resolve(*arg.type);
        if(!symbolTable.addSymbol(arg.name, argType, false)) {
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
    symbolTable.leaveScope();
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BinaryExprAST& node) {
    auto lhsType = visit(*node.lhs);
    auto rhsType = visit(*node.rhs);

    if (!lhsType || !rhsType) {
        throw std::runtime_error("Could not resolve type for one or both operands in binary expression.");
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

std::shared_ptr<Type> SemanticAnalyzer::visit(VariableExprAST& node) {
    Symbol* symbol = symbolTable.findSymbol(node.name);
    if (!symbol) {
        throw std::runtime_error("Variable '" + node.name + "' not declared.");
    }
    node.type = symbol->type;
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(FunctionCallExprAST& node) {
    Symbol* symbol = symbolTable.findSymbol(node.callee);
    if (!symbol) {
        throw std::runtime_error("Function '" + node.callee + "' not declared.");
    }

    // In a real implementation, we would check argument types and arity.
    // For now, we'll just assume the call is valid and return the function's type.
    node.type = symbol->type;
    return node.type;
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


std::shared_ptr<Type> SemanticAnalyzer::visit(TypeNameAST& node) {
    return nullptr;
}
