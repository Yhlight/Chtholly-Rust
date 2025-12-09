#include "symbol_table.h"

namespace Chtholly
{

    SymbolTable::SymbolTable()
    {
        enterScope(); // Global scope
    }

    void SymbolTable::enterScope()
    {
        scopes.emplace_back();
        lifetimeCounter++;
    }

    void SymbolTable::exitScope()
    {
        if (!scopes.empty())
        {
            for (const auto& name : scopes.back().borrowedSymbols)
            {
                SymbolInfo* info = lookup(name);
                if (info)
                {
                    info->sharedBorrowCount = 0;
                    info->mutableBorrow = false;
                }
            }
            scopes.pop_back();
        }
    }

    bool SymbolTable::define(const std::string& name, SymbolInfo& info)
    {
        if (scopes.empty())
        {
            return false;
        }
        if(isDefinedInCurrentScope(name)){
            return false;
        }
        info.lifetime = getCurrentLifetime();
        scopes.back().symbols[name] = info;
        return true;
    }

    SymbolInfo* SymbolTable::lookup(const std::string& name)
    {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            auto symbol = it->symbols.find(name);
            if (symbol != it->symbols.end())
            {
                return &symbol->second;
            }
        }
        return nullptr;
    }


    bool SymbolTable::isDefinedInCurrentScope(const std::string& name) const
    {
        if (scopes.empty())
        {
            return false;
        }
        const auto& currentScope = scopes.back().symbols;
        return currentScope.find(name) != currentScope.end();
    }

    void SymbolTable::borrow(const std::string& name)
    {
        if (!scopes.empty())
        {
            scopes.back().borrowedSymbols.push_back(name);
        }
    }

    int SymbolTable::getCurrentLifetime() const
    {
        return lifetimeCounter;
    }

} // namespace Chtholly
