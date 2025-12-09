#include "CodeGenerator.h"
#include <stdexcept>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/Casting.h>


CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("ChthollyModule", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

void CodeGenerator::generate(BlockStmtAST& ast) {
    visit(ast);
}

void CodeGenerator::dump() const {
    module->print(llvm::errs(), nullptr);
}

llvm::Type* CodeGenerator::resolveType(const TypeNameAST& typeName) {
    if (typeName.name == "i32") {
        return builder->getInt32Ty();
    }
    // Add other types here
    throw std::runtime_error("Unknown type '" + typeName.name + "' in code generator");
}


llvm::Value* CodeGenerator::visit(ASTNode& node) {
    if (auto* p = dynamic_cast<VarDeclStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<FunctionDeclAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BlockStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BinaryExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<NumberExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<VariableExprAST*>(&node)) {
        return visit(*p);
    }
    return nullptr;
}

llvm::Value* CodeGenerator::visit(VarDeclStmtAST& node) {
    llvm::Value* initVal = nullptr;
    if (node.initExpr) {
        initVal = visit(*node.initExpr);
    } else {
        // Default initialize to 0
        initVal = llvm::ConstantInt::get(*context, llvm::APInt(32, 0));
    }

    // For now, all variables are stack allocated.
    llvm::AllocaInst* alloca = builder->CreateAlloca(initVal->getType(), nullptr, node.varName);
    builder->CreateStore(initVal, alloca);
    namedValues[node.varName] = alloca;
    return alloca;
}

llvm::Value* CodeGenerator::visit(FunctionDeclAST& node) {
    llvm::Type* returnType = resolveType(*node.returnType);
    std::vector<llvm::Type*> argTypes;
    for (const auto& arg : node.args) {
        argTypes.push_back(resolveType(*arg.type));
    }

    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::Function* function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node.name, module.get());

    llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(basicBlock);

    namedValues.clear(); // Clear old variables
    visit(*node.body);

    // For now, we'll add a default return if one is not present
    if (basicBlock->getTerminator() == nullptr) {
        builder->CreateRet(llvm::ConstantInt::get(*context, llvm::APInt(32, 0, true)));
    }

    return function;
}

llvm::Value* CodeGenerator::visit(BlockStmtAST& node) {
    for (auto& stmt : node.statements) {
        if (stmt) {
            visit(*stmt);
        }
    }
    return nullptr;
}

llvm::Value* CodeGenerator::visit(BinaryExprAST& node) {
    llvm::Value* L = visit(*node.lhs);
    llvm::Value* R = visit(*node.rhs);

    if (!L || !R) {
        return nullptr;
    }

    switch (node.op) {
        case TokenType::PLUS: return builder->CreateAdd(L, R, "addtmp");
        case TokenType::MINUS: return builder->CreateSub(L, R, "subtmp");
        case TokenType::STAR: return builder->CreateMul(L, R, "multmp");
        case TokenType::SLASH: return builder->CreateSDiv(L, R, "divtmp");
        // Handle other operators
        default:
            throw std::runtime_error("Invalid binary operator");
    }
}

llvm::Value* CodeGenerator::visit(NumberExprAST& node) {
    return llvm::ConstantInt::get(*context, llvm::APInt(32, node.value, true));
}

llvm::Value* CodeGenerator::visit(VariableExprAST& node) {
    llvm::Value* v = namedValues[node.name];
    if (!v) {
        throw std::runtime_error("Unknown variable name: " + node.name);
    }
    // Load the value from the memory location
    if (auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(v)) {
        return builder->CreateLoad(alloca->getAllocatedType(), v, node.name.c_str());
    }
    return v;
}
