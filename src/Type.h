#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <vector>
#include <memory>

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
        TK_Struct
    };

    TypeKind kind;

    Type(TypeKind kind) : kind(kind) {}
    virtual ~Type() = default;

    bool isInteger() const { return kind == TK_Integer; }
    bool isFloat() const { return kind == TK_Float; }
    bool isString() const { return kind == TK_String; }
    bool isBool() const { return kind == TK_Bool; }
    // Add more checks as needed

    virtual bool isCopy() const { return false; }

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

class FunctionType : public Type {
public:
    std::shared_ptr<Type> returnType;
    std::vector<std::shared_ptr<Type>> argTypes;

    FunctionType() : Type(TK_Function) {}

    std::string toString() const override {
        return "function";
    }
};

class VoidType : public Type {
public:
    VoidType() : Type(TK_Void) {}

    std::string toString() const override {
        return "void";
    }
};


// ... other type classes can be added here ...

#endif // CHTHOLLY_TYPE_H
