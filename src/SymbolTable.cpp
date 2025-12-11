#include "SymbolTable.h"

SymbolTable::SymbolTable() : parent(nullptr) {}

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> parent) : parent(std::move(parent)) {}

bool SymbolTable::define(const std::string& name, Symbol symbol) {
    if (symbols.count(name)) {
        return false; // Symbol already exists in the current scope
    }
    symbols[name] = symbol;
    return true;
}

Symbol* SymbolTable::resolve(const std::string& name) {
    if (symbols.count(name)) {
        return &symbols[name];
    }
    if (parent != nullptr) {
        return parent->resolve(name);
    }
    return nullptr; // Symbol not found
}

std::shared_ptr<SymbolTable> SymbolTable::getParent() const {
    return parent;
}
