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
    if (typeName.is_reference) {
        // For references, we create a pointer to the resolved type.
        // We create a temporary TypeNameAST to resolve the base type.
        TypeNameAST baseTypeName(typeName.name, false);
        return resolveType(baseTypeName)->getPointerTo();
    }
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
    } else if (auto* p = dynamic_cast<BoolExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<VariableExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<FunctionCallExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ExprStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ReturnStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<IfStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<WhileStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BorrowExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<DereferenceExprAST*>(&node)) {
        return visit(*p);
    }
    return nullptr;
}

llvm::Value* CodeGenerator::visit(VarDeclStmtAST& node) {
    llvm::Type* varType = nullptr;
    if (node.type) {
        varType = resolveType(*node.type);
    }

    llvm::Value* initVal = nullptr;
    if (node.initExpr) {
        initVal = visit(*node.initExpr);
        if (!varType) {
            varType = initVal->getType();
        }
    }

    if (!varType) {
        throw std::runtime_error("Could not determine type for variable '" + node.varName + "' in code generator");
    }

    llvm::AllocaInst* alloca = builder->CreateAlloca(varType, nullptr, node.varName);

    if (initVal) {
        builder->CreateStore(initVal, alloca);
    } else {
        builder->CreateStore(llvm::Constant::getNullValue(varType), alloca);
    }

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
        case TokenType::LESS: return builder->CreateICmpSLT(L, R, "lttmp");
        case TokenType::LESS_EQUAL: return builder->CreateICmpSLE(L, R, "letmp");
        case TokenType::GREATER: return builder->CreateICmpSGT(L, R, "gttmp");
        case TokenType::GREATER_EQUAL: return builder->CreateICmpSGE(L, R, "getmp");
        case TokenType::DOUBLE_EQUAL: return builder->CreateICmpEQ(L, R, "eqtmp");
        case TokenType::NOT_EQUAL: return builder->CreateICmpNE(L, R, "netmp");
        case TokenType::PLUS_EQUAL: {
            if (auto* var = dynamic_cast<VariableExprAST*>(node.lhs.get())) {
                llvm::Value* varValue = namedValues[var->name];
                llvm::Value* newValue = builder->CreateAdd(L, R, "addtmp");
                builder->CreateStore(newValue, varValue);
                return newValue;
            }
            throw std::runtime_error("Left-hand side of assignment must be a variable.");
        }
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

llvm::Value* CodeGenerator::visit(BoolExprAST& node) {
    return llvm::ConstantInt::get(builder->getInt1Ty(), node.value);
}

llvm::Value* CodeGenerator::visit(VariableExprAST& node) {
    llvm::Value* v = namedValues[node.name];
    if (!v) {
        throw std::runtime_error("Unknown variable name: " + node.name);
    }

    if (auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(v)) {
        return builder->CreateLoad(alloca->getAllocatedType(), v, node.name.c_str());
    }
    return v;
}

llvm::Value* CodeGenerator::visit(FunctionCallExprAST& node) {
    llvm::Function* callee = module->getFunction(node.callee);
    if (!callee) {
        // A temporary hack to support printf
        if (node.callee == "println" || node.callee == "print_int") {
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
    if (node.callee == "print_int") {
        argValues.push_back(builder->CreateGlobalStringPtr("%d\n"));
        argValues.push_back(visit(*node.args[0]));
    } else {
        for (auto& arg : node.args) {
            argValues.push_back(visit(*arg));
        }
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

llvm::Value* CodeGenerator::visit(IfStmtAST& node) {
    llvm::Value* condV = visit(*node.condition);
    if (!condV) {
        return nullptr;
    }

    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(builder->getInt1Ty(), 0), "ifcond");

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont");

    builder->CreateCondBr(condV, thenBB, elseBB);

    builder->SetInsertPoint(thenBB);
    visit(*node.thenBranch);
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBB);
    }
    thenBB = builder->GetInsertBlock();

    function->insert(function->end(), elseBB);
    builder->SetInsertPoint(elseBB);
    if (node.elseBranch) {
        visit(*node.elseBranch);
    }
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBB);
    }
    elseBB = builder->GetInsertBlock();

    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);

    return nullptr;
}

llvm::Value* CodeGenerator::visit(BorrowExprAST& node) {
    if (auto* var = dynamic_cast<VariableExprAST*>(node.expr.get())) {
        llvm::Value* v = namedValues[var->name];
        if (!v) {
            throw std::runtime_error("Unknown variable name: " + var->name);
        }
        return v;
    }
    throw std::runtime_error("Cannot take the address of a non-variable expression.");
}

llvm::Value* CodeGenerator::visit(DereferenceExprAST& node) {
    llvm::Value* expr = visit(*node.expr);
    return builder->CreateLoad(expr->getType()->getPointerElementType(), expr, "deref");
}

llvm::Value* CodeGenerator::visit(WhileStmtAST& node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopCondBB = llvm::BasicBlock::Create(*context, "loopcond", function);
    llvm::BasicBlock* loopBodyBB = llvm::BasicBlock::Create(*context, "loopbody");
    llvm::BasicBlock* loopEndBB = llvm::BasicBlock::Create(*context, "loopend");

    builder->CreateBr(loopCondBB);
    builder->SetInsertPoint(loopCondBB);

    llvm::Value* condV = visit(*node.condition);
    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(builder->getInt1Ty(), 0), "whilecond");
    builder->CreateCondBr(condV, loopBodyBB, loopEndBB);

    function->insert(function->end(), loopBodyBB);
    builder->SetInsertPoint(loopBodyBB);
    visit(*node.body);
    builder->CreateBr(loopCondBB);

    function->insert(function->end(), loopEndBB);
    builder->SetInsertPoint(loopEndBB);

    return nullptr;
}
