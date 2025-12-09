#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <iostream>
#include <vector>

int main() {
    llvm::LLVMContext context;
    llvm::Module module("ChthollyHelloWorld", context);
    llvm::IRBuilder<> builder(context);

    // Declare the printf function
    llvm::Type *i8_ptr_type = builder.getInt8Ty()->getPointerTo();
    llvm::FunctionType *printf_type = llvm::FunctionType::get(builder.getInt32Ty(), {i8_ptr_type}, true);
    llvm::Function *printf_func = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, "printf", &module);

    // Define the main function
    llvm::FunctionType *main_type = llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function *main_func = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, "main", &module);
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", main_func);
    builder.SetInsertPoint(entry);

    // Create the "Hello, World!" string
    llvm::Value *hello_world_str = builder.CreateGlobalStringPtr("Hello, Chtholly!\\n");

    // Call printf
    std::vector<llvm::Value *> args;
    args.push_back(hello_world_str);
    builder.CreateCall(printf_func, args);

    // Return 0
    builder.CreateRet(builder.getInt32(0));

    // Verify the module
    llvm::verifyModule(module, &llvm::errs());

    // Print the LLVM IR
    module.print(llvm::outs(), nullptr);

    return 0;
}
