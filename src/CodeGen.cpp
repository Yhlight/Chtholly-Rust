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

    // If the value is an alloca, it's a mutable variable, so we load from it.
    if (auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(v)) {
        return context.getBuilder().CreateLoad(alloca->getAllocatedType(), v, m_name.c_str());
    } else {
        // Otherwise, it's an immutable value, so we can just return it.
        return v;
    }
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

    llvm::Type* type = nullptr;
    if (m_type) {
        type = m_type->getLLVMType(context);
    } else {
        // If type is not specified, infer it from the initializer.
        type = initVal->getType();
    }

    if (m_isMutable) {
        // For mutable variables, allocate memory on the stack.
        llvm::AllocaInst* alloca = context.getBuilder().CreateAlloca(type, nullptr, m_varName.c_str());
        context.getBuilder().CreateStore(initVal, alloca);
        context.m_namedValues[m_varName] = alloca;
    } else {
        // For immutable variables, just keep the value in the symbol table.
        context.m_namedValues[m_varName] = initVal;
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
    } else {
        return logErrorV("Expression is not a struct");
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
