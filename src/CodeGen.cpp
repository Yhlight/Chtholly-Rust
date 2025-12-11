#include "CodeGen.h"
#include "AST.h"
#include "llvm/IR/Verifier.h"

namespace chtholly {

CodeGen::CodeGen() : m_builder(m_context) {
    m_module = std::make_unique<llvm::Module>("ChthollyJIT", m_context);
}

void CodeGen::generate(FunctionAST& ast) {
    ast.codegen(*this);
}

llvm::Value* logErrorV(const char* str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

llvm::Value* NumberExprAST::codegen(CodeGen& context) {
    return llvm::ConstantFP::get(context.getContext(), llvm::APFloat(m_val));
}

llvm::Value* VariableExprAST::codegen(CodeGen& context) {
    llvm::Value* v = context.m_namedValues[m_name];
    if (!v) {
        return logErrorV("Unknown variable name");
    }
    return context.getBuilder().CreateLoad(llvm::Type::getDoubleTy(context.getContext()), v, m_name.c_str());
}

llvm::Value* BinaryExprAST::codegen(CodeGen& context) {
    llvm::Value *L = m_lhs->codegen(context);
    llvm::Value *R = m_rhs->codegen(context);
    if (!L || !R)
        return nullptr;

    switch (m_op) {
    case '+':
        return context.getBuilder().CreateFAdd(L, R, "addtmp");
    case '-':
        return context.getBuilder().CreateFSub(L, R, "subtmp");
    case '*':
        return context.getBuilder().CreateFMul(L, R, "multmp");
    case '/':
        return context.getBuilder().CreateFDiv(L, R, "divtmp");
    default:
        return logErrorV("invalid binary operator");
    }
}

llvm::Value* LetExprAST::codegen(CodeGen& context) {
    llvm::Value* initVal = m_init->codegen(context);
    if (!initVal)
        return nullptr;

    llvm::AllocaInst* alloca = context.getBuilder().CreateAlloca(llvm::Type::getDoubleTy(context.getContext()), 0, m_varName.c_str());
    context.getBuilder().CreateStore(initVal, alloca);

    // Add the variable to the symbol table.
    context.m_namedValues[m_varName] = alloca;

    return initVal;
}

llvm::Function* PrototypeAST::codegen(CodeGen& context) {
    std::vector<llvm::Type*> doubles(m_args.size(), llvm::Type::getDoubleTy(context.getContext()));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(context.getContext()), doubles, false);
    llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, m_name, context.getModule());

    unsigned idx = 0;
    for (auto& arg : f->args()) {
        arg.setName(m_args[idx++]);
    }
    return f;
}

llvm::Function* FunctionAST::codegen(CodeGen& context) {
    llvm::Function* theFunction = context.getModule().getFunction(m_proto->getName());
    if (!theFunction) {
        theFunction = m_proto->codegen(context);
    }

    if (!theFunction) {
        return nullptr;
    }

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context.getContext(), "entry", theFunction);
    context.getBuilder().SetInsertPoint(bb);

    context.m_namedValues.clear();
    for (auto& arg : theFunction->args()) {
        context.m_namedValues[std::string(arg.getName())] = &arg;
    }

    llvm::Value* lastVal = nullptr;
    for (auto& expr : m_body) {
        lastVal = expr->codegen(context);
    }

    if (lastVal) {
        context.getBuilder().CreateRet(lastVal);
        llvm::verifyFunction(*theFunction);
        return theFunction;
    }

    theFunction->eraseFromParent();
    return nullptr;
}

} // namespace chtholly
