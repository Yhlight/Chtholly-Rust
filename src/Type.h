#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>

namespace Chtholly {

class Type {
public:
    enum TypeID {
        IntegerTyID,
        FloatTyID,
        VoidTyID,
        BoolTyID,
    };

    TypeID getTypeID() const { return ID; }

    bool isIntegerTy() const { return getTypeID() == IntegerTyID; }
    bool isFloatTy() const { return getTypeID() == FloatTyID; }
    bool isVoidTy() const { return getTypeID() == VoidTyID; }
    bool isBoolTy() const { return getTypeID() == BoolTyID; }

    static Type* getIntegerTy();
    static Type* getFloatTy();
    static Type* getVoidTy();
    static Type* getBoolTy();

private:
    TypeID ID;
    Type(TypeID ID) : ID(ID) {}
};

} // namespace Chtholly

#endif // CHTHOLLY_TYPE_H
