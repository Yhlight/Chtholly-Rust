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
    scopeStack.back()[name] = {name, std::move(type), isMutable};
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
