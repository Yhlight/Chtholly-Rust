#include "SemanticAnalyzer.h"
#include <stdexcept>

void SemanticAnalyzer::visit(const ProgramAST& node) {
    for (const auto& stmt : node.getStatements()) {
        if (const auto* letStmt = dynamic_cast<const LetStmtAST*>(stmt.get())) {
            visit(*letStmt);
        }
    }
}

void SemanticAnalyzer::visit(const LetStmtAST& node) {
    Type declaredType = node.getType();
    Type valueType = visit(*node.getValue());

    if (declaredType.getBuiltinType() != valueType.getBuiltinType()) {
        throw std::runtime_error("Type mismatch in variable declaration");
    }

    Symbol symbol(node.getName(), false, declaredType);
    if (!symbolTable.add(symbol)) {
        throw std::runtime_error("Duplicate variable declaration: " + node.getName());
    }
}

Type SemanticAnalyzer::visit(const ExprAST& node) {
    if (const auto* numExpr = dynamic_cast<const NumberExprAST*>(&node)) {
        return visit(*numExpr);
    }
    if (const auto* floatExpr = dynamic_cast<const FloatExprAST*>(&node)) {
        return visit(*floatExpr);
    }
    throw std::runtime_error("Unknown expression type");
}

Type SemanticAnalyzer::visit(const NumberExprAST& node) {
    return node.getType();
}

Type SemanticAnalyzer::visit(const FloatExprAST& node) {
    return node.getType();
}
