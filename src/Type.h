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
    // Add more checks as needed

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
};

class FloatType : public Type {
public:
    int bitwidth; // 32 for float, 64 for double

    FloatType(int bitwidth) : Type(TK_Float), bitwidth(bitwidth) {}

    std::string toString() const override {
        return "f" + std::to_string(bitwidth);
    }
};

class StringType : public Type {
public:
    StringType() : Type(TK_String) {}

    std::string toString() const override {
        return "string";
    }
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


// ... other type classes can be added here ...

#endif // CHTHOLLY_TYPE_H
