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
    };

    TypeID getTypeID() const { return ID; }

    bool isIntegerTy() const { return getTypeID() == IntegerTyID; }
    bool isFloatTy() const { return getTypeID() == FloatTyID; }
    bool isVoidTy() const { return getTypeID() == VoidTyID; }

    static Type* getIntegerTy();
    static Type* getFloatTy();
    static Type* getVoidTy();

private:
    TypeID ID;
    Type(TypeID ID) : ID(ID) {}
};

} // namespace Chtholly

#endif // CHTHOLLY_TYPE_H
