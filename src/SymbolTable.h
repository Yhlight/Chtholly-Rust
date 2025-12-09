#ifndef CHTHOLLY_SYMBOLTABLE_H
#define CHTHOLLY_SYMBOLTABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Type.h"

// Information about a declared symbol
struct Symbol {
    std::string name;
    std::shared_ptr<Type> type;
    bool isMutable;
};

// A single scope, mapping names to symbols
using Scope = std::unordered_map<std::string, Symbol>;

// Manages a stack of scopes for symbol resolution
class SymbolTable {
public:
    SymbolTable();

    void enterScope();
    void leaveScope();

    // Tries to add a symbol to the current scope.
    // Returns true on success, false if the symbol already exists in the current scope.
    bool addSymbol(const std::string& name, std::shared_ptr<Type> type, bool isMutable);

    // Finds a symbol by searching from the innermost scope outwards.
    // Returns a pointer to the Symbol if found, otherwise nullptr.
    Symbol* findSymbol(const std::string& name);

private:
    std::vector<Scope> scopeStack;
};

#endif // CHTHOLLY_SYMBOLTABLE_H
