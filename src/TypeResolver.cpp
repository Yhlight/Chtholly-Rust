#include "TypeResolver.h"
#include <stdexcept>
#include "AST.h"
#include "SymbolTable.h"

TypeResolver::TypeResolver(SymbolTable& symbolTable) : symbolTable(symbolTable) {}

std::shared_ptr<Type> TypeResolver::resolve(const TypeNameAST& typeName) {
    if (auto* refType = dynamic_cast<const ReferenceTypeAST*>(&typeName)) {
        return std::make_shared<ReferenceType>(resolve(*refType->referencedType), refType->isMutable);
    }

    if (typeName.name == "i32") {
        return std::make_shared<IntegerType>(32, true);
    }
    if (typeName.name == "f64") {
        return std::make_shared<FloatType>(64);
    }
    if (typeName.name == "string") {
        return std::make_shared<StringType>();
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
