#include "SemanticAnalyzer.h"
#include <iostream>

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Stmt>>& statements) {
    currentScope = std::make_shared<SymbolTable>();
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
}

void SemanticAnalyzer::visit(NumberExpr& expr) {
    // For now, we assume all numbers are integers.
}

void SemanticAnalyzer::visit(VariableExpr& expr) {
    if (currentScope->resolve(expr.token.value) == nullptr) {
        throw std::runtime_error("Undefined variable '" + expr.token.value + "'.");
    }
}

void SemanticAnalyzer::visit(BinaryExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SemanticAnalyzer::visit(LetStmt& stmt) {
    if (!currentScope->define(stmt.name.value, {SymbolType::VARIABLE, TokenType::INTEGER})) {
        throw std::runtime_error("Variable '" + stmt.name.value + "' already declared in this scope.");
    }
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }
}

void SemanticAnalyzer::visit(ReturnStmt& stmt) {
    if (stmt.value) {
        stmt.value->accept(*this);
    }
}

void SemanticAnalyzer::visit(FnDecl& stmt) {
    if (!currentScope->define(stmt.name.value, {SymbolType::FUNCTION, TokenType::FN})) {
         throw std::runtime_error("Function '" + stmt.name.value + "' already declared in this scope.");
    }
    auto previousScope = currentScope;
    currentScope = std::make_shared<SymbolTable>(previousScope);

    for (const auto& param : stmt.params) {
        currentScope->define(param.value, {SymbolType::VARIABLE, TokenType::INTEGER});
    }

    for (const auto& statement : stmt.body) {
        statement->accept(*this);
    }

    currentScope = previousScope;
}

void SemanticAnalyzer::visit(ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
}
