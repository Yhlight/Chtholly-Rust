#ifndef CHTHOLLY_PROGRAMAST_H
#define CHTHOLLY_PROGRAMAST_H

#include "ASTNode.h"
#include "StmtAST.h"
#include <vector>
#include <memory>

class ProgramAST : public ASTNode {
    std::vector<std::unique_ptr<StmtAST>> statements;
public:
    ProgramAST(std::vector<std::unique_ptr<StmtAST>> statements)
        : statements(std::move(statements)) {}

    const std::vector<std::unique_ptr<StmtAST>>& getStatements() const {
        return statements;
    }
};

#endif //CHTHOLLY_PROGRAMAST_H
