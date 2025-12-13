#include "Type.h"
#include <map>
#include <utility>

namespace Chtholly {

Type* Type::getIntegerTy() {
    static Type IntTy(IntegerTyID);
    return &IntTy;
}

Type* Type::getFloatTy() {
    static Type FloatTy(FloatTyID);
    return &FloatTy;
}

Type* Type::getVoidTy() {
    static Type VoidTy(VoidTyID);
    return &VoidTy;
}

Type* Type::getBoolTy() {
    static Type boolTy(BoolTyID);
    return &boolTy;
}

Type* Type::getStringTy() {
    static Type StringTy(StringTyID);
    return &StringTy;
}

Type* Type::getReferenceTy(Type* baseType, bool isMutable) {
    static std::map<std::pair<Type*, bool>, ReferenceType> refTypes;
    return &refTypes.try_emplace({baseType, isMutable}, baseType, isMutable).first->second;
}

} // namespace Chtholly
