#ifndef CHTHOLLY_CODEGEN_H
#define CHTHOLLY_CODEGEN_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include <map>
#include <string>

namespace chtholly {

// Forward declare the AST nodes we will generate code for.
class ExprAST;
class NumberExprAST;
class VariableExprAST;
class PrototypeAST;
class FunctionAST;
class Type;
class StructDefAST;

enum class OwnershipState {
    Valid,
    Moved
};

struct Symbol {
    llvm::Value* value;
    Type* type;
    OwnershipState state;
};

class CodeGen {
public:
    CodeGen();

    // The main entry point for code generation.
    void generate(FunctionAST& ast);

    // Getters for the LLVM objects.
    llvm::LLVMContext& getContext() { return m_context; }
    llvm::IRBuilder<>& getBuilder() { return m_builder; }
    llvm::Module& getModule() { return *m_module; }

    // Symbol table for variables.
    std::map<std::string, Symbol> m_namedValues;

    // Type table for structs.
    std::map<std::string, llvm::StructType*> m_structTypes;

    // Struct definition table.
    std::map<std::string, StructDefAST*> m_structDefs;

    // Type ownership.
    std::vector<std::unique_ptr<Type>> m_ownedTypes;

private:
    llvm::LLVMContext m_context;
    llvm::IRBuilder<> m_builder;
    std::unique_ptr<llvm::Module> m_module;
};

} // namespace chtholly

#endif // CHTHOLLY_CODEGEN_H
