#include "CodeGen.h"
#include "AST.h"
#include "llvm/IR/Verifier.h"

namespace chtholly {

llvm::Type* I32Type::getLLVMType(CodeGen& context) {
    return llvm::Type::getInt32Ty(context.getContext());
}

llvm::Type* F64Type::getLLVMType(CodeGen& context) {
    return llvm::Type::getDoubleTy(context.getContext());
}

llvm::Type* BoolType::getLLVMType(CodeGen& context) {
    return llvm::Type::getInt1Ty(context.getContext());
}

llvm::Type* CharType::getLLVMType(CodeGen& context) {
    return llvm::Type::getInt8Ty(context.getContext());
}

llvm::Type* VoidType::getLLVMType(CodeGen& context) {
    return llvm::Type::getVoidTy(context.getContext());
}

llvm::Type* StructType::getLLVMType(CodeGen& context) {
    return context.m_structTypes[m_name];
}

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
    if (m_type == NumberType::Int) {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.getContext()), m_intVal, true);
    } else {
        return llvm::ConstantFP::get(context.getContext(), llvm::APFloat(m_floatVal));
    }
}

llvm::Value* VariableExprAST::codegen(CodeGen& context) {
    auto it = context.m_namedValues.find(m_name);
    if (it == context.m_namedValues.end()) {
        return logErrorV("Unknown variable name");
    }

    Symbol& symbol = it->second;

    if (symbol.state == OwnershipState::Moved) {
        return logErrorV("use of moved value");
    }

    // If the value is an alloca, it's a mutable variable or a struct, so we load from it.
    if (auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(symbol.value)) {
        if (alloca->getAllocatedType()->isStructTy()) {
            return alloca;
        }
        return context.getBuilder().CreateLoad(alloca->getAllocatedType(), symbol.value, m_name.c_str());
    } else {
        // Otherwise, it's an immutable value, so we can just return it.
        return symbol.value;
    }
}

llvm::Value* BinaryExprAST::codegen(CodeGen& context) {
    llvm::Value *L = m_lhs->codegen(context);
    llvm::Value *R = m_rhs->codegen(context);
    if (!L || !R)
        return nullptr;

    bool isFloat = L->getType()->isFloatingPointTy() || R->getType()->isFloatingPointTy();

    if (isFloat) {
        if (!L->getType()->isFloatingPointTy()) {
            L = context.getBuilder().CreateSIToFP(L, llvm::Type::getDoubleTy(context.getContext()), "sitofp");
        }
        if (!R->getType()->isFloatingPointTy()) {
            R = context.getBuilder().CreateSIToFP(R, llvm::Type::getDoubleTy(context.getContext()), "sitofp");
        }
    }

    switch (m_op) {
    case '+':
        return isFloat ? context.getBuilder().CreateFAdd(L, R, "addtmp") : context.getBuilder().CreateAdd(L, R, "addtmp");
    case '-':
        return isFloat ? context.getBuilder().CreateFSub(L, R, "subtmp") : context.getBuilder().CreateSub(L, R, "subtmp");
    case '*':
        return isFloat ? context.getBuilder().CreateFMul(L, R, "multmp") : context.getBuilder().CreateMul(L, R, "multmp");
    case '/':
        return isFloat ? context.getBuilder().CreateFDiv(L, R, "divtmp") : context.getBuilder().CreateSDiv(L, R, "divtmp");
    case '<':
        return isFloat ? context.getBuilder().CreateFCmpULT(L, R, "cmptmp") : context.getBuilder().CreateICmpULT(L, R, "cmptmp");
    case '>':
        return isFloat ? context.getBuilder().CreateFCmpUGT(L, R, "cmptmp") : context.getBuilder().CreateICmpUGT(L, R, "cmptmp");
    default:
        return logErrorV("invalid binary operator");
    }
}

llvm::Value* LetExprAST::codegen(CodeGen& context) {
    llvm::Value* initVal = m_init->codegen(context);
    if (!initVal)
        return nullptr;

    Type* varType = nullptr;
    if (m_type) {
        varType = m_type.get();
    } else if (auto* varExpr = dynamic_cast<VariableExprAST*>(m_init.get())) {
        auto it = context.m_namedValues.find(varExpr->getName());
        if (it != context.m_namedValues.end()) {
            varType = it->second.type;
        }
    }

    if (!varType) {
        // Fallback for literals, etc.
        // This is a simplification. A real type system would be more robust.
        if (initVal->getType()->isIntegerTy()) {
            auto newType = std::make_unique<I32Type>();
            varType = newType.get();
            context.m_ownedTypes.push_back(std::move(newType));
        } else if (initVal->getType()->isDoubleTy()) {
            auto newType = std::make_unique<F64Type>();
            varType = newType.get();
            context.m_ownedTypes.push_back(std::move(newType));
        }
    }

    llvm::Type* llvmType = varType ? varType->getLLVMType(context) : initVal->getType();

    if (auto* varExpr = dynamic_cast<VariableExprAST*>(m_init.get())) {
        auto it = context.m_namedValues.find(varExpr->getName());
        if (it != context.m_namedValues.end()) {
            Symbol& symbol = it->second;
            if (dynamic_cast<StructType*>(symbol.type)) {
                symbol.state = OwnershipState::Moved;
            }
        }
    }

    if (initVal->getType() != llvmType) {
        if (llvmType->isIntegerTy() && initVal->getType()->isFloatingPointTy()) {
            initVal = context.getBuilder().CreateFPToSI(initVal, llvmType, "fptosi");
        } else if (llvmType->isFloatingPointTy() && initVal->getType()->isIntegerTy()) {
            initVal = context.getBuilder().CreateSIToFP(initVal, llvmType, "sitofp");
        }
    }

    if (m_isMutable || llvmType->isStructTy()) {
        // For mutable variables or structs, allocate memory on the stack.
        llvm::AllocaInst* alloca = context.getBuilder().CreateAlloca(llvmType, nullptr, m_varName.c_str());
        if (llvmType->isStructTy()) {
            initVal = context.getBuilder().CreateLoad(llvmType, initVal);
        }
        context.getBuilder().CreateStore(initVal, alloca);
        context.m_namedValues[m_varName] = {alloca, varType, OwnershipState::Valid};
    } else {
        // For immutable variables, just keep the value in the symbol table.
        context.m_namedValues[m_varName] = {initVal, varType, OwnershipState::Valid};
    }

    return initVal;
}

llvm::Value* IfExprAST::codegen(CodeGen& context) {
    llvm::Value* condV = m_cond->codegen(context);
    if (!condV)
        return nullptr;

    // Convert condition to a bool by comparing non-equal to 0.
    if (condV->getType()->isFloatingPointTy()) {
        condV = context.getBuilder().CreateFCmpONE(condV, llvm::ConstantFP::get(context.getContext(), llvm::APFloat(0.0)), "ifcond");
    } else {
        condV = context.getBuilder().CreateICmpNE(condV, llvm::ConstantInt::get(condV->getType(), 0), "ifcond");
    }

    llvm::Function* theFunction = context.getBuilder().GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context.getContext(), "then", theFunction);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context.getContext(), "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context.getContext(), "ifcont");

    context.getBuilder().CreateCondBr(condV, thenBB, elseBB);

    // Emit then value.
    context.getBuilder().SetInsertPoint(thenBB);
    llvm::Value* thenV = nullptr;
    for (auto& expr : m_then) {
        thenV = expr->codegen(context);
    }
    context.getBuilder().CreateBr(mergeBB);
    thenBB = context.getBuilder().GetInsertBlock();

    // Emit else block.
    theFunction->getBasicBlockList().push_back(elseBB);
    context.getBuilder().SetInsertPoint(elseBB);
    llvm::Value* elseV = nullptr;
    for (auto& expr : m_else) {
        elseV = expr->codegen(context);
    }
    context.getBuilder().CreateBr(mergeBB);
    elseBB = context.getBuilder().GetInsertBlock();

    // Emit merge block.
    theFunction->getBasicBlockList().push_back(mergeBB);
    context.getBuilder().SetInsertPoint(mergeBB);

    if (thenV && elseV) {
        if (thenV->getType() != elseV->getType()) {
            return logErrorV("if expression must have same type for then and else blocks");
        }
        llvm::PHINode* pn = context.getBuilder().CreatePHI(thenV->getType(), 2, "iftmp");
        pn->addIncoming(thenV, thenBB);
        pn->addIncoming(elseV, elseBB);
        return pn;
    }

    // For now, if expressions return void if one of the blocks is empty.
    return llvm::Constant::getNullValue(llvm::Type::getVoidTy(context.getContext()));
}

llvm::Value* StructDefAST::codegen(CodeGen& context) {
    std::vector<llvm::Type*> memberTypes;
    for (const auto& member : m_members) {
        memberTypes.push_back(member.second->getLLVMType(context));
    }

    llvm::StructType* structType = llvm::StructType::create(context.getContext(), memberTypes, m_name);
    context.m_structTypes[m_name] = structType;
    context.m_structDefs[m_name] = this;

    // Struct definitions don't generate a value.
    return nullptr;
}

llvm::Value* StructInitExprAST::codegen(CodeGen& context) {
    llvm::StructType* structType = context.m_structTypes[m_structName];
    if (!structType) {
        return logErrorV("Unknown struct type");
    }

    llvm::AllocaInst* alloca = context.getBuilder().CreateAlloca(structType, nullptr, m_structName.c_str());

    for (int i = 0; i < m_args.size(); ++i) {
        llvm::Value* argVal = m_args[i]->codegen(context);
        if (!argVal)
            return nullptr;

        llvm::Value* memberPtr = context.getBuilder().CreateStructGEP(structType, alloca, i);
        context.getBuilder().CreateStore(argVal, memberPtr);
    }

    return alloca;
}

llvm::Value* MemberAccessExprAST::codegen(CodeGen& context) {
    llvm::Value* structVal = m_structExpr->codegen(context);
    if (!structVal)
        return nullptr;

    llvm::Type* structType = structVal->getType();
    if (auto* ptrType = llvm::dyn_cast<llvm::PointerType>(structType)) {
        structType = ptrType->getElementType();
    }

    llvm::StructType* sType = llvm::dyn_cast<llvm::StructType>(structType);
    if (!sType) {
        return logErrorV("Expression is not a struct");
    }

    auto it = context.m_structDefs.find(std::string(sType->getName()));
    if (it == context.m_structDefs.end()) {
        return logErrorV("Unknown struct type");
    }

    StructDefAST* structDef = it->second;
    int memberIndex = -1;
    for (int i = 0; i < structDef->getMembers().size(); ++i) {
        if (structDef->getMembers()[i].first == m_memberName) {
            memberIndex = i;
            break;
        }
    }

    if (memberIndex == -1) {
        return logErrorV("Unknown member");
    }

    llvm::Value* memberPtr = context.getBuilder().CreateStructGEP(sType, structVal, memberIndex, m_memberName.c_str());
    return context.getBuilder().CreateLoad(sType->getElementType(memberIndex), memberPtr, m_memberName.c_str());
}


llvm::Function* PrototypeAST::codegen(CodeGen& context) {
    std::vector<llvm::Type*> argTypes;
    for (const auto& arg : m_args) {
        argTypes.push_back(arg.second->getLLVMType(context));
    }

    llvm::Type* returnType = m_returnType->getLLVMType(context);

    llvm::FunctionType* ft = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, m_name, context.getModule());

    unsigned idx = 0;
    for (auto& arg : f->args()) {
        arg.setName(m_args[idx++].first);
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
    auto& protoArgs = m_proto->getArgs();
    int i = 0;
    for (auto& arg : theFunction->args()) {
        context.m_namedValues[std::string(arg.getName())] = {&arg, protoArgs[i++].second.get(), OwnershipState::Valid};
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
