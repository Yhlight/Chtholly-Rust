#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <vector>
#include <memory>

namespace Chtholly {

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Represents a function declaration
class FunctionDeclAST : public ASTNode {
public:
    FunctionDeclAST(const std::string& name) : name(name) {}

    const std::string& getName() const { return name; }

private:
    std::string name;
    // For now, we'll keep it simple. We'll add parameters, return type, and body later.
};

// Represents the root of the AST, which is a collection of top-level declarations
class TranslationUnitAST {
public:
    void addFunction(std::unique_ptr<FunctionDeclAST> function) {
        if(function) {
            functions.push_back(std::move(function));
        }
    }

    const std::vector<std::unique_ptr<FunctionDeclAST>>& getFunctions() const {
        return functions;
    }

private:
    std::vector<std::unique_ptr<FunctionDeclAST>> functions;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
