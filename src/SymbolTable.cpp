#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    // Start with a global scope
    enterScope();
}

void SymbolTable::enterScope() {
    scopeStack.emplace_back();
}

void SymbolTable::leaveScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop_back();
    }
}

bool SymbolTable::addSymbol(const std::string& name, std::shared_ptr<Type> type, bool isMutable) {
    if (scopeStack.empty()) {
        return false; // Should always have at least the global scope
    }

    Scope& currentScope = scopeStack.back();
    if (currentScope.count(name)) {
        return false; // Symbol already exists in the current scope
    }

    currentScope[name] = Symbol{name, std::move(type), isMutable};
    return true;
}

Symbol* SymbolTable::findSymbol(const std::string& name) {
    // Search from the innermost scope (the end of the vector) to the outermost
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        auto& scope = *it;
        if (scope.count(name)) {
            return &scope.at(name);
        }
    }
    return nullptr; // Symbol not found
}
