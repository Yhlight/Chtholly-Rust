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
        StringTyID,
    ReferenceTyID,
    };

    TypeID getTypeID() const { return ID; }

    bool isIntegerTy() const { return getTypeID() == IntegerTyID; }
    bool isFloatTy() const { return getTypeID() == FloatTyID; }
    bool isVoidTy() const { return getTypeID() == VoidTyID; }
    bool isBoolTy() const { return getTypeID() == BoolTyID; }
    bool isStringTy() const { return getTypeID() == StringTyID; }
bool isReferenceTy() const { return getTypeID() == ReferenceTyID; }

    static Type* getIntegerTy();
    static Type* getFloatTy();
    static Type* getVoidTy();
    static Type* getBoolTy();
    static Type* getStringTy();
static Type* getReferenceTy(Type* baseType, bool isMutable);

protected:
    TypeID ID;
    Type(TypeID ID) : ID(ID) {}
};

class ReferenceType : public Type {
    Type* baseType;
    bool m_isMutable;

public:
    ReferenceType(Type* baseType, bool isMutable)
        : Type(ReferenceTyID), baseType(baseType), m_isMutable(isMutable) {}

    Type* getBaseType() const { return baseType; }
    bool isMutable() const { return m_isMutable; }
};

} // namespace Chtholly

#endif // CHTHOLLY_TYPE_H
