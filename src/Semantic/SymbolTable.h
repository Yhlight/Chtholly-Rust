#ifndef CHTHOLLY_SYMBOLTABLE_H
#define CHTHOLLY_SYMBOLTABLE_H

#include "Symbol.h"
#include <map>
#include <string>
#include <optional>

class SymbolTable {
public:
    bool add(const Symbol& symbol);
    std::optional<Symbol> lookup(const std::string& name) const;

private:
    std::map<std::string, Symbol> symbols;
};

#endif //CHTHOLLY_SYMBOLTABLE_H
