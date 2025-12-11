#ifndef CHTHOLLY_EXPRAST_H
#define CHTHOLLY_EXPRAST_H

#include "ASTNode.h"
#include "../Type/Type.h"
#include <string>

class ExprAST : public ASTNode {
    Type type;
public:
    ExprAST(Type type) : type(type) {}
    Type getType() const { return type; }
};

class NumberExprAST : public ExprAST {
    int value;
public:
    NumberExprAST(int value) : ExprAST(Type(BuiltinType::I32)), value(value) {}
    int getValue() const { return value; }
};

class FloatExprAST : public ExprAST {
    double value;
public:
    FloatExprAST(double value) : ExprAST(Type(BuiltinType::F64)), value(value) {}
    double getValue() const { return value; }
};

#endif //CHTHOLLY_EXPRAST_H
