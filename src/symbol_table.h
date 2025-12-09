#ifndef CHTHOLLY_SYMBOL_TABLE_H
#define CHTHOLLY_SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Chtholly
{
    template <typename T>
    class SymbolTable
    {
    public:
        SymbolTable() { enterScope(); }

        void enterScope() { scopes.emplace_back(); }
        void exitScope() { scopes.pop_back(); }

        bool define(const std::string& name, const T& info)
        {
            if (scopes.back().find(name) != scopes.back().end())
            {
                return false;
            }
            scopes.back()[name] = info;
            return true;
        }

        T* lookup(const std::string& name)
        {
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
            {
                auto& scope = *it;
                auto symbolIt = scope.find(name);
                if (symbolIt != scope.end())
                {
                    return &symbolIt->second;
                }
            }
            return nullptr;
        }

        bool isDefinedInCurrentScope(const std::string& name) const
        {
            return scopes.back().count(name) > 0;
        }

    private:
        std::vector<std::unordered_map<std::string, T>> scopes;
    };

} // namespace Chtholly

#endif // CHTHOLLY_SYMBOL_TABLE_H
