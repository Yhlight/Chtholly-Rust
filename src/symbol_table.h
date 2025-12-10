#ifndef CHTHOLLY_SYMBOL_TABLE_H
#define CHTHOLLY_SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Chtholly
{

    enum class SymbolState
    {
        Valid,
        Moved
    };

    enum class SymbolType
    {
        Variable,
        Function,
        Struct,
        Class,
        Enum
    };

    struct SymbolInfo
    {
        std::string type;
        bool isMutable;
        SymbolState state;
        SymbolType symbolType = SymbolType::Variable;
        std::vector<std::string> parameterTypes;
        std::string returnType;
        int sharedBorrowCount = 0;
        bool mutableBorrow = false;
        int lifetime = 0;
        std::unordered_map<std::string, SymbolInfo> fields;
        std::unordered_map<std::string, std::vector<std::string>> variants;

        SymbolInfo() = default;
        SymbolInfo(std::string type, bool isMutable, SymbolState state, SymbolType symbolType = SymbolType::Variable)
            : type(std::move(type)), isMutable(isMutable), state(state), symbolType(symbolType) {}
    };

    class SymbolTable
    {
    public:
        SymbolTable();

        void enterScope();
        void exitScope();

        bool define(const std::string& name, SymbolInfo& info);
        SymbolInfo* lookup(const std::string& name);
        bool isDefinedInCurrentScope(const std::string& name) const;
        void borrow(const std::string& name);
        int getCurrentLifetime() const;


    private:
        struct Scope {
            std::unordered_map<std::string, SymbolInfo> symbols;
            std::vector<std::string> borrowedSymbols;
        };
        std::vector<Scope> scopes;
        // Lifetime counter, a larger value indicates a more nested scope with a shorter lifetime.
        int lifetimeCounter = 0;
    };

} // namespace Chtholly

#endif // CHTHOLLY_SYMBOL_TABLE_H
