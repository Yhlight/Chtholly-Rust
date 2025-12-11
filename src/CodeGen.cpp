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

llvm::Value* NumberExprAST::codegen(CodeGen& context) {
    return llvm::ConstantFP::get(context.getContext(), llvm::APFloat(m_val));
}

// Helper to create a basic error message.
llvm::Value* logErrorV(const char* str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

llvm::Value* VariableExprAST::codegen(CodeGen& context) {
    llvm::Value* v = context.m_namedValues[m_name];
    if (!v) {
        return logErrorV("Unknown variable name");
    }
    return v;
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

    if (llvm::Value* retVal = m_body->codegen(context)) {
        context.getBuilder().CreateRet(retVal);
        llvm::verifyFunction(*theFunction);
        return theFunction;
    }

    theFunction->eraseFromParent();
    return nullptr;
}

} // namespace chtholly
