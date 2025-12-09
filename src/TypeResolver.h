#ifndef CHTHOLLY_TYPERESOLVER_H
#define CHTHOLLY_TYPERESOLVER_H

#include "Type.h"
#include "AST.h"
#include <memory>
#include <string>

class TypeResolver {
public:
    std::shared_ptr<Type> resolve(const TypeNameAST& typeName);
};

#endif // CHTHOLLY_TYPERESOLVER_H
