#ifndef CHTHOLLY_SYMBOL_TABLE_H
#define CHTHOLLY_SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Chtholly
{

    enum class SymbolState
    {
        Valid,
        Moved
    };

    struct SymbolInfo
    {
        std::string type;
        bool isMutable;
        SymbolState state;
    };

    class SymbolTable
    {
    public:
        SymbolTable();

        void enterScope();
        void exitScope();

        bool define(const std::string& name, const SymbolInfo& info);
        SymbolInfo* lookup(const std::string& name);
        bool isDefinedInCurrentScope(const std::string& name) const;


    private:
        std::vector<std::unordered_map<std::string, SymbolInfo>> scopes;
    };

} // namespace Chtholly

#endif // CHTHOLLY_SYMBOL_TABLE_H
