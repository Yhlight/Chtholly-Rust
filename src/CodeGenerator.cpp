#include "CodeGenerator.h"
#include <stdexcept>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/Casting.h>


CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("ChthollyModule", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    declarePrintf();
}

void CodeGenerator::generate(BlockStmtAST& ast) {
    visit(ast);
}

void CodeGenerator::dump() const {
    module->print(llvm::errs(), nullptr);
}

void CodeGenerator::declarePrintf() {
    llvm::Type* i8_ptr_type = builder->getInt8Ty()->getPointerTo();
    llvm::FunctionType* printf_type = llvm::FunctionType::get(builder->getInt32Ty(), {i8_ptr_type}, true);
    module->getOrInsertFunction("printf", printf_type);
}


llvm::Type* CodeGenerator::resolveType(const TypeNameAST& typeName) {
    if (typeName.name == "i32") {
        return builder->getInt32Ty();
    }
    if (typeName.name == "f64") {
        return builder->getDoubleTy();
    }
    if (typeName.name == "string") {
        return builder->getInt8Ty()->getPointerTo();
    }
    if (typeName.name == "bool") {
        return builder->getInt1Ty();
    }
    if (typeName.name == "void") {
        return builder->getVoidTy();
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
    } else if (auto* p = dynamic_cast<StringExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<VariableExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<FunctionCallExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ExprStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ReturnStmtAST*>(&node)) {
        return visit(*p);
    }
    return nullptr;
}

llvm::Value* CodeGenerator::visit(VarDeclStmtAST& node) {
    llvm::Value* initVal = nullptr;
    if (node.initExpr) {
        initVal = visit(*node.initExpr);
    } else {
        // This should be caught by the semantic analyzer
        throw std::runtime_error("Variable declaration without initializer in code generator");
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
    unsigned i = 0;
    for (auto& arg : function->args()) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(arg.getType(), nullptr, node.args[i].name);
        builder->CreateStore(&arg, alloca);
        namedValues[node.args[i].name] = alloca;
        i++;
    }

    visit(*node.body);

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
    if (node.type->isFloat()) {
        return llvm::ConstantFP::get(*context, llvm::APFloat(node.value));
    }
    return llvm::ConstantInt::get(*context, llvm::APInt(32, node.value, true));
}

llvm::Value* CodeGenerator::visit(StringExprAST& node) {
    return builder->CreateGlobalStringPtr(node.value);
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

llvm::Value* CodeGenerator::visit(FunctionCallExprAST& node) {
    llvm::Function* callee = module->getFunction(node.callee);
    if (!callee) {
        // A temporary hack to support printf
        if (node.callee == "println") {
            callee = module->getFunction("printf");
            if (!callee) throw std::runtime_error("Could not find printf function");
        } else {
            throw std::runtime_error("Unknown function referenced: " + node.callee);
        }
    }

    if (callee->arg_size() != node.args.size() && callee->isVarArg() == false) {
        throw std::runtime_error("Incorrect number of arguments passed to function " + node.callee);
    }

    std::vector<llvm::Value*> argValues;
    for (auto& arg : node.args) {
        argValues.push_back(visit(*arg));
    }

    return builder->CreateCall(callee, argValues, "calltmp");
}

llvm::Value* CodeGenerator::visit(ExprStmtAST& node) {
    return visit(*node.expr);
}

llvm::Value* CodeGenerator::visit(ReturnStmtAST& node) {
    if (node.returnValue) {
        return builder->CreateRet(visit(*node.returnValue));
    } else {
        return builder->CreateRetVoid();
    }
}
