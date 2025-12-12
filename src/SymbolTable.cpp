#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    // Start with a single global scope for both symbols and types
    scopeStack.emplace_back();
    typeScopeStack.emplace_back();
}

void SymbolTable::enterScope() {
    scopeStack.emplace_back();
    typeScopeStack.emplace_back();
}

void SymbolTable::leaveScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop_back();
    }
    if (!typeScopeStack.empty()) {
        typeScopeStack.pop_back();
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
    Symbol newSymbol = {name, std::move(type), isMutable};
    newSymbol.lifetime = scopeStack.size() - 1;
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

size_t SymbolTable::getCurrentScopeIndex() const {
    return scopeStack.size() - 1;
}

bool SymbolTable::add_type(const std::string& name, std::shared_ptr<Type> type) {
    if (typeScopeStack.empty()) {
        return false; // Should not happen
    }
    if (typeScopeStack.back().count(name)) {
        return false;
    }
    typeScopeStack.back()[name] = std::move(type);
    return true;
}

std::shared_ptr<Type> SymbolTable::find_type(const std::string& name) {
    for (auto it = typeScopeStack.rbegin(); it != typeScopeStack.rend(); ++it) {
        auto typeIt = it->find(name);
        if (typeIt != it->end()) {
            return typeIt->second;
        }
    }
    return nullptr;
}
