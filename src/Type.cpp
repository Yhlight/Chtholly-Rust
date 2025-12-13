#include "Type.h"

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

} // namespace Chtholly
