#include "CodeGenerator.h"
#include "AST.h"
#include "Type.h"
#include "llvm/IR/Verifier.h"

namespace Chtholly {

llvm::Type* CodeGenerator::llvmTypeFromChthollyType(const Type* type) {
    if (type->isIntegerTy()) {
        return llvm::Type::getInt32Ty(*context);
    } else if (type->isFloatTy()) {
        return llvm::Type::getDoubleTy(*context);
    } else if (type->isVoidTy()) {
        return llvm::Type::getVoidTy(*context);
    } else if (type->isBoolTy()) {
        return llvm::Type::getInt1Ty(*context);
    }
    return nullptr;
}

CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("ChthollyJIT", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

void CodeGenerator::generate(const FunctionAST& function) {
    function.accept(*this);
}

void CodeGenerator::dump() const {
    module->print(llvm::errs(), nullptr);
}

void CodeGenerator::visit(const NumberExprAST& node) {
    if (node.getValue() == (int)node.getValue()) {
        lastValue = llvm::ConstantInt::get(*context, llvm::APInt(32, node.getValue(), true));
    } else {
        lastValue = llvm::ConstantFP::get(*context, llvm::APFloat(node.getValue()));
    }
}

void CodeGenerator::visit(const VariableExprAST& node) {
    llvm::Value* v = namedValues[node.getName()];
    if (!v) {
        // TODO: Error handling
    }
    lastValue = builder->CreateLoad(llvmTypeFromChthollyType(node.getType()), v, node.getName());
}

void CodeGenerator::visit(const BinaryExprAST& node) {
    node.getLHS()->accept(*this);
    llvm::Value* L = lastValue;
    node.getRHS()->accept(*this);
    llvm::Value* R = lastValue;

    if (!L || !R) {
        // TODO: Error handling
        return;
    }

    if (node.getType()->isIntegerTy()) {
        switch (node.getOp()) {
            case TokenType::Plus:
                lastValue = builder->CreateAdd(L, R, "addtmp");
                break;
            case TokenType::Minus:
                lastValue = builder->CreateSub(L, R, "subtmp");
                break;
            case TokenType::Star:
                lastValue = builder->CreateMul(L, R, "multmp");
                break;
            case TokenType::Slash:
                lastValue = builder->CreateSDiv(L, R, "divtmp");
                break;
            case TokenType::Less:
                lastValue = builder->CreateICmpSLT(L, R, "cmptmp");
                break;
            case TokenType::Greater:
                lastValue = builder->CreateICmpSGT(L, R, "cmptmp");
                break;
            case TokenType::EqualEqual:
                lastValue = builder->CreateICmpEQ(L, R, "cmptmp");
                break;
            default:
                // TODO: Error handling
                break;
        }
    } else if (node.getType()->isFloatTy()) {
        switch (node.getOp()) {
            case TokenType::Plus:
                lastValue = builder->CreateFAdd(L, R, "addtmp");
                break;
            case TokenType::Minus:
                lastValue = builder->CreateFSub(L, R, "subtmp");
                break;
            case TokenType::Star:
                lastValue = builder->CreateFMul(L, R, "multmp");
                break;
            case TokenType::Slash:
                lastValue = builder->CreateFDiv(L, R, "divtmp");
                break;
            case TokenType::Less:
                lastValue = builder->CreateFCmpULT(L, R, "cmptmp");
                break;
            case TokenType::Greater:
                lastValue = builder->CreateFCmpUGT(L, R, "cmptmp");
                break;
            case TokenType::EqualEqual:
                lastValue = builder->CreateFCmpUEQ(L, R, "cmptmp");
                break;
            default:
                // TODO: Error handling
                break;
        }
    }
}

void CodeGenerator::visit(const CallExprAST& node) {
    // TODO: Implement
}

void CodeGenerator::visit(const PrototypeAST& node) {
    std::vector<llvm::Type*> ArgTypes;
    for (const auto& arg : node.getArgs()) {
        ArgTypes.push_back(llvmTypeFromChthollyType(arg.second));
    }

    llvm::Type* retType = llvmTypeFromChthollyType(node.getReturnType());
    llvm::FunctionType* funcType = llvm::FunctionType::get(retType, ArgTypes, false);

    llvm::Function* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, node.getName(), module.get());

    unsigned i = 0;
    for (auto& arg : func->args()) {
        arg.setName(node.getArgs()[i++].first);
    }

    lastValue = func;
}

void CodeGenerator::visit(const FunctionAST& node) {
    node.getProto()->accept(*this);
    llvm::Function* func = static_cast<llvm::Function*>(lastValue);

    llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(basicBlock);

    namedValues.clear();
    for (auto& arg : func->args()) {
        namedValues[std::string(arg.getName())] = &arg;
    }

    for (const auto& stmt : node.getBody()) {
        stmt->accept(*this);
    }

    llvm::verifyFunction(*func);
    lastValue = func;
}

void CodeGenerator::visit(const VarDeclStmtAST& node) {
    node.getInit()->accept(*this);
    llvm::Value* initVal = lastValue;
    if (!initVal) {
        // TODO: Error handling
        return;
    }

    llvm::Function* func = builder->GetInsertBlock()->getParent();
    llvm::IRBuilder<> TmpB(&func->getEntryBlock(), func->getEntryBlock().begin());
    llvm::AllocaInst* alloca = TmpB.CreateAlloca(llvmTypeFromChthollyType(node.getType()), 0, node.getName());

    builder->CreateStore(initVal, alloca);
    namedValues[node.getName()] = alloca;
}

void CodeGenerator::visit(const ReturnStmtAST& node) {
    if (node.getValue()) {
        node.getValue()->accept(*this);
        builder->CreateRet(lastValue);
    } else {
        builder->CreateRetVoid();
    }
}

void CodeGenerator::visit(const ExprStmtAST& node) {
    node.getExpr()->accept(*this);
}

void CodeGenerator::visit(const IfStmtAST& node) {
    node.getCond()->accept(*this);
    llvm::Value* condV = lastValue;
    if (!condV) {
        // TODO: Error handling
        return;
    }

    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), 0, true), "ifcond");

    llvm::Function* func = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", func);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont");

    builder->CreateCondBr(condV, thenBB, elseBB);

    builder->SetInsertPoint(thenBB);
    for (const auto& stmt : node.getThen()) {
        stmt->accept(*this);
    }
    builder->CreateBr(mergeBB);
    thenBB = builder->GetInsertBlock();

    func->insert(func->end(), elseBB);
    builder->SetInsertPoint(elseBB);
    for (const auto& stmt : node.getElse()) {
        stmt->accept(*this);
    }
    builder->CreateBr(mergeBB);
    elseBB = builder->GetInsertBlock();

    func->insert(func->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);
}

} // namespace Chtholly
