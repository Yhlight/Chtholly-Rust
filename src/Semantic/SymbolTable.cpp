#include "SymbolTable.h"

bool SymbolTable::add(const Symbol& symbol) {
    if (symbols.count(symbol.getName())) {
        return false; // Symbol already exists
    }
    symbols.emplace(symbol.getName(), symbol);
    return true;
}

std::optional<Symbol> SymbolTable::lookup(const std::string& name) const {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    return std::nullopt;
}
