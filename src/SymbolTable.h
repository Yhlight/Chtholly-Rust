#ifndef CHTHOLLY_SYMBOLTABLE_H
#define CHTHOLLY_SYMBOLTABLE_H

#include <string>
#include <unordered_map>
#include <memory>
#include "Token.h"

enum class SymbolType {
    VARIABLE,
    FUNCTION
};

struct Symbol {
    SymbolType type;
    TokenType dataType; // For variables
    // Other info like function signature can be added later
};

class SymbolTable {
public:
    SymbolTable();
    explicit SymbolTable(std::shared_ptr<SymbolTable> parent);

    bool define(const std::string& name, Symbol symbol);
    Symbol* resolve(const std::string& name);

    std::shared_ptr<SymbolTable> getParent() const;

private:
    std::unordered_map<std::string, Symbol> symbols;
    std::shared_ptr<SymbolTable> parent;
};

#endif //CHTHOLLY_SYMBOLTABLE_H
