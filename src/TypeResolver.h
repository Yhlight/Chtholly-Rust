#ifndef CHTHOLLY_TYPERESOLVER_H
#define CHTHOLLY_TYPERESOLVER_H

#include "Type.h"
#include "AST.h"
#include "SemanticAnalyzer.h"
#include <memory>
#include <string>

class SymbolTable;
class LifetimeManager;

class TypeResolver {
public:
    TypeResolver(SymbolTable& symbolTable, LifetimeManager& lifetimeManager);
    std::shared_ptr<Type> resolve(const TypeNameAST& typeName);
    std::shared_ptr<Type> resolve(const EnumDeclAST& enumDecl);
private:
    SymbolTable& symbolTable;
    LifetimeManager& lifetimeManager;
};

#endif // CHTHOLLY_TYPERESOLVER_H
