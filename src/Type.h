#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class Type {
public:
    enum TypeKind {
        TK_Integer,
        TK_Float,
        TK_String,
        TK_Char,
        TK_Bool,
        TK_Void,
        TK_Function,
        TK_Class,
        TK_Struct,
        TK_Reference,
        TK_Array,
        TK_DynamicArray,
        TK_Enum
    };

    TypeKind kind;

    Type(TypeKind kind) : kind(kind) {}
    virtual ~Type() = default;

    bool isInteger() const { return kind == TK_Integer; }
    bool isFloat() const { return kind == TK_Float; }
    bool isString() const { return kind == TK_String; }
    bool isBool() const { return kind == TK_Bool; }
    bool isStruct() const { return kind == TK_Struct; }
    bool isClass() const { return kind == TK_Class; }
    // Add more checks as needed

    virtual bool isCopy() const { return false; }
    virtual bool isArray() const { return false; }
    virtual bool isDynamicArray() const { return false; }
    virtual bool isEnum() const { return false; }

    virtual std::string toString() const = 0;
};

class IntegerType : public Type {
public:
    int bitwidth;
    bool isSigned;

    IntegerType(int bitwidth, bool isSigned)
        : Type(TK_Integer), bitwidth(bitwidth), isSigned(isSigned) {}

    std::string toString() const override {
        return (isSigned ? "i" : "u") + std::to_string(bitwidth);
    }

    bool isCopy() const override { return true; }
};

class FloatType : public Type {
public:
    int bitwidth; // 32 for float, 64 for double

    FloatType(int bitwidth) : Type(TK_Float), bitwidth(bitwidth) {}

    std::string toString() const override {
        return "f" + std::to_string(bitwidth);
    }

    bool isCopy() const override { return true; }
};

class StringType : public Type {
public:
    StringType() : Type(TK_String) {}

    std::string toString() const override {
        return "string";
    }
};

class BoolType : public Type {
public:
    BoolType() : Type(TK_Bool) {}

    std::string toString() const override {
        return "bool";
    }

    bool isCopy() const override { return true; }
};

class CharType : public Type {
public:
    CharType() : Type(TK_Char) {}

    std::string toString() const override {
        return "char";
    }

    bool isCopy() const override { return true; }
};

class StructType : public Type {
public:
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> members;

    StructType(std::string name, std::vector<std::pair<std::string, std::shared_ptr<Type>>> members)
        : Type(TK_Struct), name(std::move(name)), members(std::move(members)) {}

    std::string toString() const override { return name; }
};

class FunctionType;

class ClassType : public Type {
public:
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> members;
    std::unordered_map<std::string, std::shared_ptr<FunctionType>> methods;

    ClassType(std::string name, std::vector<std::pair<std::string, std::shared_ptr<Type>>> members, std::unordered_map<std::string, std::shared_ptr<FunctionType>> methods)
        : Type(TK_Class), name(std::move(name)), members(std::move(members)), methods(std::move(methods)) {}

    std::string toString() const override { return name; }
};

class FunctionType : public Type {
public:
    std::shared_ptr<Type> returnType;
    std::vector<std::shared_ptr<Type>> argTypes;

    FunctionType() : Type(TK_Function) {}

    std::string toString() const override {
        return "function";
    }
};

class MethodType : public FunctionType {
public:
    std::shared_ptr<ClassType> parentClass;
    MethodType(std::shared_ptr<ClassType> parentClass, std::shared_ptr<FunctionType> funcType)
        : parentClass(std::move(parentClass)) {
        returnType = funcType->returnType;
        argTypes = funcType->argTypes;
    }
};

class VoidType : public Type {
public:
    VoidType() : Type(TK_Void) {}

    std::string toString() const override {
        return "void";
    }
};

class ReferenceType : public Type {
public:
    std::shared_ptr<Type> referencedType;
    bool isMutable;

    ReferenceType(std::shared_ptr<Type> referencedType, bool isMutable)
        : Type(TK_Reference), referencedType(std::move(referencedType)), isMutable(isMutable) {}

    std::string toString() const override {
        return std::string("&") + (isMutable ? "mut " : "") + referencedType->toString();
    }

    bool isCopy() const override { return true; }
};


class ArrayType : public Type {
public:
    std::shared_ptr<Type> elementType;
    int size;

    ArrayType(std::shared_ptr<Type> elementType, int size)
        : Type(TK_Array), elementType(std::move(elementType)), size(size) {}

    std::string toString() const override {
        return elementType->toString() + "[" + std::to_string(size) + "]";
    }
    bool isArray() const override { return true; }
};

class DynamicArrayType : public Type {
public:
    std::shared_ptr<Type> elementType;

    DynamicArrayType(std::shared_ptr<Type> elementType)
        : Type(TK_DynamicArray), elementType(std::move(elementType)) {}

    std::string toString() const override {
        return elementType->toString() + "[]";
    }
    bool isDynamicArray() const override { return true; }
};

class EnumType : public Type {
public:
    std::string name;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Type>>> variants;

    EnumType(std::string name, std::unordered_map<std::string, std::vector<std::shared_ptr<Type>>> variants)
        : Type(TK_Enum), name(std::move(name)), variants(std::move(variants)) {}

    std::string toString() const override { return name; }
    bool isEnum() const override { return true; }
};

// ... other type classes can be added here ...

#endif // CHTHOLLY_TYPE_H
