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
    Symbol symbol(node.getName(), false, node.getType());
    if (!symbolTable.add(symbol)) {
        throw std::runtime_error("Duplicate variable declaration: " + node.getName());
    }
}
