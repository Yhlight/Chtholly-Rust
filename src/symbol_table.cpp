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
    }

    void SymbolTable::exitScope()
    {
        if (!scopes.empty())
        {
            scopes.pop_back();
        }
    }

    bool SymbolTable::define(const std::string& name, const SymbolInfo& info)
    {
        if (scopes.empty())
        {
            return false;
        }
        if(isDefinedInCurrentScope(name)){
            return false;
        }
        scopes.back()[name] = info;
        return true;
    }

    SymbolInfo* SymbolTable::lookup(const std::string& name)
    {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            auto symbol = it->find(name);
            if (symbol != it->end())
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
        const auto& currentScope = scopes.back();
        return currentScope.find(name) != currentScope.end();
    }


} // namespace Chtholly
