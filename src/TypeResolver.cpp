#include "TypeResolver.h"
#include <stdexcept>

std::shared_ptr<Type> TypeResolver::resolve(const TypeNameAST& typeName) {
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
    // Add other built-in types here
    throw std::runtime_error("Unknown type '" + typeName.name + "'");
}
