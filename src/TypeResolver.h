#ifndef CHTHOLLY_TYPERESOLVER_H
#define CHTHOLLY_TYPERESOLVER_H

#include "Type.h"
#include "AST.h"
#include <memory>
#include <string>

class SymbolTable;

class TypeResolver {
public:
    TypeResolver(SymbolTable& symbolTable);
    std::shared_ptr<Type> resolve(const TypeNameAST& typeName);
    std::shared_ptr<Type> resolve(const EnumDeclAST& enumDecl);
private:
    SymbolTable& symbolTable;
};

#endif // CHTHOLLY_TYPERESOLVER_H
