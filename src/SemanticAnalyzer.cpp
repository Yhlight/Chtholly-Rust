#include "SemanticAnalyzer.h"
#include <stdexcept>

namespace Chtholly {

void SemanticAnalyzer::analyze(const FunctionAST& function) {
    function.accept(*this);
}

void SemanticAnalyzer::visit(const NumberExprAST& node) {
    // For simplicity, we'll assume all numbers are i32 for now.
    // This will be expanded later.
    currentExprType = "i32";
}

void SemanticAnalyzer::visit(const StringExprAST& node) {
    currentExprType = "string";
}

void SemanticAnalyzer::visit(const VariableExprAST& node) {
    if (symbolTable.find(node.getName()) == symbolTable.end()) {
        throw std::runtime_error("Undeclared variable: " + node.getName());
    }
    currentExprType = symbolTable[node.getName()];
}

void SemanticAnalyzer::visit(const BinaryExprAST& node) {
    node.getLHS()->accept(*this);
    std::string leftType = currentExprType;
    node.getRHS()->accept(*this);
    std::string rightType = currentExprType;

    if (leftType != rightType) {
        throw std::runtime_error("Type mismatch in binary expression");
    }
    currentExprType = leftType;
}

void SemanticAnalyzer::visit(const CallExprAST& node) {
    // Nothing to do for now
}

void SemanticAnalyzer::visit(const PrototypeAST& node) {
    // Nothing to do for now
}

void SemanticAnalyzer::visit(const FunctionAST& node) {
    symbolTable.clear();
    for (const auto& arg : node.getProto()->getArgs()) {
        symbolTable[arg.first] = arg.second;
    }

    for (const auto& stmt : node.getBody()) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(const VarDeclStmtAST& node) {
    if (symbolTable.find(node.getName()) != symbolTable.end()) {
        throw std::runtime_error("Variable already declared: " + node.getName());
    }

    node.getInit()->accept(*this);
    std::string initType = currentExprType;

    if (node.getType() != initType) {
        throw std::runtime_error("Type mismatch in variable declaration for '" + node.getName() + "'. Expected " + node.getType() + " but got " + initType);
    }

    symbolTable[node.getName()] = node.getType();
}

} // namespace Chtholly
