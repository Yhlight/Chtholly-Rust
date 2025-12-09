#include "SemanticAnalyzer.h"
#include <iostream>
#include <stdexcept>

SemanticAnalyzer::SemanticAnalyzer() {}

void SemanticAnalyzer::analyze(const BlockStmtAST& ast) {
    visit(ast);
}

void SemanticAnalyzer::visit(const ASTNode& node) {
    // This is a bit of a hack to dispatch to the correct visit method.
    // A more robust implementation would use a more sophisticated visitor pattern.
    if (auto* p = dynamic_cast<const VarDeclStmtAST*>(&node)) {
        visit(*p);
    } else if (auto* p = dynamic_cast<const FunctionDeclAST*>(&node)) {
        visit(*p);
    } else if (auto* p = dynamic_cast<const BlockStmtAST*>(&node)) {
        visit(*p);
    } else if (auto* p = dynamic_cast<const BinaryExprAST*>(&node)) {
        visit(*p);
    } else if (auto* p = dynamic_cast<const NumberExprAST*>(&node)) {
        visit(*p);
    } else if (auto* p = dynamic_cast<const VariableExprAST*>(&node)) {
        visit(*p);
    } else if (auto* p = dynamic_cast<const TypeNameAST*>(&node)) {
        visit(*p);
    }
}

void SemanticAnalyzer::visit(const VarDeclStmtAST& node) {
    if (node.initExpr) {
        visit(*node.initExpr);
    }
    // A real implementation would have a way to resolve the type name string
    // into an actual type object, but for now we'll just create a dummy one.
    auto type = std::make_unique<TypeNameAST>("unknown"); // Placeholder
    if (!symbolTable.addSymbol(node.varName, std::move(type), node.isMutable)) {
        throw std::runtime_error("Variable '" + node.varName + "' already declared in this scope.");
    }
}

void SemanticAnalyzer::visit(const FunctionDeclAST& node) {
    // A real implementation would handle function overloading and types properly.
    auto returnType = std::make_unique<TypeNameAST>(node.returnType->name);
    if (!symbolTable.addSymbol(node.name, std::move(returnType), false)) {
        throw std::runtime_error("Function '" + node.name + "' already declared in this scope.");
    }

    symbolTable.enterScope();
    for (const auto& arg : node.args) {
        auto argType = std::make_unique<TypeNameAST>(arg.type->name);
        if(!symbolTable.addSymbol(arg.name, std::move(argType), false)) {
             throw std::runtime_error("Argument '" + arg.name + "' already declared in this scope.");
        }
    }
    visit(*node.body);
    symbolTable.leaveScope();
}

void SemanticAnalyzer::visit(const BlockStmtAST& node) {
    symbolTable.enterScope();
    for (const auto& stmt : node.statements) {
        if(stmt) visit(*stmt);
    }
    symbolTable.leaveScope();
}

void SemanticAnalyzer::visit(const BinaryExprAST& node) {
    visit(*node.lhs);
    visit(*node.rhs);
}

void SemanticAnalyzer::visit(const NumberExprAST& node) {
    // Nothing to do for numbers
}

void SemanticAnalyzer::visit(const VariableExprAST& node) {
    if (!symbolTable.findSymbol(node.name)) {
        throw std::runtime_error("Variable '" + node.name + "' not declared.");
    }
}

void SemanticAnalyzer::visit(const TypeNameAST& node) {
    // In a real compiler, we would check if this is a valid type name.
}
