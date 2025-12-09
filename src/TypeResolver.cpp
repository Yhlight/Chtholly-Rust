#include "TypeResolver.h"
#include <stdexcept>

std::shared_ptr<Type> TypeResolver::resolve(const TypeNameAST& typeName) {
    std::shared_ptr<Type> resolvedType;
    if (typeName.name == "i32") {
        resolvedType = std::make_shared<IntegerType>(32, true);
    } else if (typeName.name == "f64") {
        resolvedType = std::make_shared<FloatType>(64);
    } else if (typeName.name == "string") {
        resolvedType = std::make_shared<StringType>();
    } else {
        // Add other built-in types here
        throw std::runtime_error("Unknown type '" + typeName.name + "'");
    }

    if (typeName.is_reference) {
        return std::make_shared<ReferenceType>(resolvedType);
    }
    return resolvedType;
}
