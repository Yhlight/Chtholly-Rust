#include "SemanticAnalyzer.h"
#include <iostream>
#include <stdexcept>

SemanticAnalyzer::SemanticAnalyzer() {}

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

    // For now, if there is an initializer, we'll infer the type from it.
    // This is a simplification. A real implementation would handle explicit type annotations.
    if (!initType) {
        // This would be an error if explicit types are not provided.
        // For now, we'll default to i32.
        initType = std::make_shared<IntegerType>(32, true);
    }

    if (!symbolTable.addSymbol(node.varName, initType, node.isMutable)) {
        throw std::runtime_error("Variable '" + node.varName + "' already declared in this scope.");
    }
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(FunctionDeclAST& node) {
    auto returnType = typeResolver.resolve(*node.returnType);
    if (!symbolTable.addSymbol(node.name, returnType, false)) {
        throw std::runtime_error("Function '" + node.name + "' already declared in this scope.");
    }

    symbolTable.enterScope();
    for (auto& arg : node.args) {
        auto argType = typeResolver.resolve(*arg.type);
        if(!symbolTable.addSymbol(arg.name, argType, false)) {
             throw std::runtime_error("Argument '" + arg.name + "' already declared in this scope.");
        }
    }
    visit(*node.body);
    symbolTable.leaveScope();
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

    // For now, we only support numeric types in binary expressions
    if ((!lhsType->isInteger() && !lhsType->isFloat()) || (!rhsType->isInteger() && !rhsType->isFloat())) {
        throw std::runtime_error("Binary operator applied to non-numeric type.");
    }

    // For now, we require the types to be identical.
    // A real implementation would handle type promotion (e.g., int + float).
    if (lhsType->toString() != rhsType->toString()) {
        throw std::runtime_error("Type mismatch in binary expression: " + lhsType->toString() + " vs " + rhsType->toString());
    }

    node.type = lhsType; // The type of the expression is the type of its operands
    return lhsType;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(NumberExprAST& node) {
    // A simplistic way to distinguish between int and float literals.
    if (std::to_string(node.value).find('.') != std::string::npos) {
        node.type = std::make_shared<FloatType>(64);
    } else {
        node.type = std::make_shared<IntegerType>(32, true);
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

std::shared_ptr<Type> SemanticAnalyzer::visit(TypeNameAST& node) {
    // This node is used for parsing, but during semantic analysis we resolve it to a Type object.
    // The actual resolution happens in the calling context (e.g., VarDecl, FunctionDecl).
    return nullptr;
}
