#ifndef CHTHOLLY_STMTAST_H
#define CHTHOLLY_STMTAST_H

#include "ASTNode.h"
#include "ExprAST.h"
#include <string>
#include <memory>

class StmtAST : public ASTNode {};

class LetStmtAST : public StmtAST {
    std::string name;
    std::unique_ptr<ExprAST> value;
public:
    LetStmtAST(const std::string& name, std::unique_ptr<ExprAST> value)
        : name(name), value(std::move(value)) {}
    const std::string& getName() const { return name; }
    const ExprAST* getValue() const { return value.get(); }
};

#endif //CHTHOLLY_STMTAST_H
