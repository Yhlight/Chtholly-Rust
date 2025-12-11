#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    // Start with a single global scope
    scopeStack.emplace_back();
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
        return false; // Should not happen
    }
    // Check if the symbol already exists in the current scope
    if (scopeStack.back().count(name)) {
        return false;
    }
    Symbol newSymbol;
    newSymbol.name = name;
    newSymbol.type = std::move(type);
    newSymbol.isMutable = isMutable;
    newSymbol.isMoved = false;
    newSymbol.immutableBorrows = 0;
    newSymbol.mutableBorrow = false;
    newSymbol.borrowedInScope = false;
    newSymbol.scopeLevel = scopeStack.size() - 1;
    newSymbol.lifetimeScopeLevel = scopeStack.size() - 1;
    scopeStack.back()[name] = newSymbol;
    return true;
}

Symbol* SymbolTable::findSymbol(const std::string& name) {
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        auto symbolIt = it->find(name);
        if (symbolIt != it->end()) {
            return &symbolIt->second;
        }
    }
    return nullptr; // Symbol not found
}

Scope& SymbolTable::getCurrentScope() {
    return scopeStack.back();
}

bool SymbolTable::add_type(const std::string& name, std::shared_ptr<Type> type) {
    if (types.count(name)) {
        return false;
    }
    types[name] = std::move(type);
    return true;
}

std::shared_ptr<Type> SymbolTable::find_type(const std::string& name) {
    if (types.count(name)) {
        return types[name];
    }
    return nullptr;
}
