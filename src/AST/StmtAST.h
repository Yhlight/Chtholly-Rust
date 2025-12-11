#ifndef CHTHOLLY_STMTAST_H
#define CHTHOLLY_STMTAST_H

#include "ASTNode.h"
#include "ExprAST.h"
#include "../Type/Type.h"
#include <string>
#include <memory>

class StmtAST : public ASTNode {};

class LetStmtAST : public StmtAST {
    std::string name;
    Type type;
    std::unique_ptr<ExprAST> value;
public:
    LetStmtAST(const std::string& name, Type type, std::unique_ptr<ExprAST> value)
        : name(name), type(type), value(std::move(value)) {}
    const std::string& getName() const { return name; }
    Type getType() const { return type; }
    const ExprAST* getValue() const { return value.get(); }
};

#endif //CHTHOLLY_STMTAST_H
