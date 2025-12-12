#include "SemanticAnalyzer.h"
#include <stdexcept>

namespace Chtholly {

void SemanticAnalyzer::analyze(const FunctionAST& function) {
    function.accept(*this);
}

void SemanticAnalyzer::visit(const NumberExprAST& node) {
    // Nothing to do
}

void SemanticAnalyzer::visit(const VariableExprAST& node) {
    if (symbolTable.find(node.getName()) == symbolTable.end()) {
        throw std::runtime_error("Undeclared variable: " + node.getName());
    }
}

void SemanticAnalyzer::visit(const BinaryExprAST& node) {
    node.getLHS()->accept(*this);
    node.getRHS()->accept(*this);
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
    symbolTable[node.getName()] = node.getType();
    node.getInit()->accept(*this);
}

} // namespace Chtholly
