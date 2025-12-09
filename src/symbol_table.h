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
        int sharedBorrowCount = 0;
        bool mutableBorrow = false;

        // For functions
        std::vector<std::string> paramTypes;
        std::string returnType;
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
        void borrow(const std::string& name);


    private:
        struct Scope {
            std::unordered_map<std::string, SymbolInfo> symbols;
            std::vector<std::string> borrowedSymbols;
        };
        std::vector<Scope> scopes;
    };

} // namespace Chtholly

#endif // CHTHOLLY_SYMBOL_TABLE_H
