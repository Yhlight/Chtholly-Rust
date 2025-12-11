#ifndef CHTHOLLY_EXPRAST_H
#define CHTHOLLY_EXPRAST_H

#include "ASTNode.h"
#include <string>

class ExprAST : public ASTNode {};

class NumberExprAST : public ExprAST {
    int value;
public:
    NumberExprAST(int value) : value(value) {}
    int getValue() const { return value; }
};

#endif //CHTHOLLY_EXPRAST_H
