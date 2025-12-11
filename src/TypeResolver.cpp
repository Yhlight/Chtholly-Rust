#include "TypeResolver.h"
#include <stdexcept>
#include "AST.h"
#include "SymbolTable.h"

TypeResolver::TypeResolver(SymbolTable& symbolTable, LifetimeManager& lifetimeManager)
    : symbolTable(symbolTable), lifetimeManager(lifetimeManager) {}

std::shared_ptr<Type> TypeResolver::resolve(const TypeNameAST& typeName) {
    if (auto* refType = dynamic_cast<const ReferenceTypeAST*>(&typeName)) {
        return std::make_shared<ReferenceType>(resolve(*refType->referencedType), refType->isMutable, lifetimeManager.get_current_lifetime());
    }

    if (auto* arrayType = dynamic_cast<const ArrayTypeAST*>(&typeName)) {
        auto elementType = resolve(*arrayType->elementType);
        if (arrayType->size) {
            if (auto* sizeExpr = dynamic_cast<NumberExprAST*>(arrayType->size.get())) {
                return std::make_shared<ArrayType>(elementType, sizeExpr->value);
            }
            throw std::runtime_error("Array size must be an integer literal for now.");
        } else {
            return std::make_shared<DynamicArrayType>(elementType);
        }
    }

    if (typeName.name == "i32") {
        return std::make_shared<IntegerType>(32, true);
    }
    if (typeName.name == "f64") {
        return std::make_shared<FloatType>(64);
    }
    if (typeName.name == "void") {
        return std::make_shared<VoidType>();
    }

    if (auto type = symbolTable.find_type(typeName.name)) {
        return type;
    }

    // Add other built-in types here
    throw std::runtime_error("Unknown type '" + typeName.name + "'");
}

std::shared_ptr<Type> TypeResolver::resolve(const EnumDeclAST& enumDecl) {
    auto type = symbolTable.find_type(enumDecl.name);
    if (!type) {
        throw std::runtime_error("Unknown enum type '" + enumDecl.name + "'");
    }
    return type;
}
