#include "CodeGenerator.h"
#include <llvm/IR/Verifier.h>

CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("ChthollyJIT", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

std::string CodeGenerator::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    std::string ir_string;
    llvm::raw_string_ostream ostream(ir_string);
    module->print(ostream, nullptr);
    return ostream.str();
}

void CodeGenerator::visit(NumberExpr& expr) {
    lastValue = llvm::ConstantInt::get(*context, llvm::APInt(32, std::stoi(expr.token.value), true));
}

void CodeGenerator::visit(VariableExpr& expr) {
    lastValue = namedValues[expr.token.value];
    if (!lastValue) {
        throw std::runtime_error("Unknown variable name: " + expr.token.value);
    }
    lastValue = builder->CreateLoad(llvm::Type::getInt32Ty(*context), lastValue, expr.token.value.c_str());
}

void CodeGenerator::visit(BinaryExpr& expr) {
    expr.left->accept(*this);
    llvm::Value* L = lastValue;
    expr.right->accept(*this);
    llvm::Value* R = lastValue;

    if (!L || !R) {
        throw std::runtime_error("Invalid binary expression");
    }

    switch (expr.op.type) {
        case TokenType::PLUS:
            lastValue = builder->CreateAdd(L, R, "addtmp");
            break;
        case TokenType::MINUS:
            lastValue = builder->CreateSub(L, R, "subtmp");
            break;
        case TokenType::STAR:
            lastValue = builder->CreateMul(L, R, "multmp");
            break;
        case TokenType::GREATER:
            lastValue = builder->CreateICmpSGT(L, R, "cmptmp");
            break;
        default:
            throw std::runtime_error("Invalid binary operator");
    }
}

void CodeGenerator::visit(LetStmt& stmt) {
    llvm::Function *TheFunction = builder->GetInsertBlock()->getParent();
    llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    llvm::AllocaInst* Alloca = TmpB.CreateAlloca(llvm::Type::getInt32Ty(*context), 0, stmt.name.value.c_str());

    if (stmt.initializer) {
        stmt.initializer->accept(*this);
        builder->CreateStore(lastValue, Alloca);
    }
    namedValues[stmt.name.value] = Alloca;
}


void CodeGenerator::visit(ReturnStmt& stmt) {
    if (stmt.value) {
        stmt.value->accept(*this);
        builder->CreateRet(lastValue);
    } else {
        builder->CreateRetVoid();
    }
}

void CodeGenerator::visit(FnDecl& stmt) {
    std::vector<llvm::Type*> Ints(stmt.params.size(), llvm::Type::getInt32Ty(*context));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), Ints, false);
    llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, stmt.name.value, module.get());

    unsigned Idx = 0;
    for (auto &Arg : f->args()) {
        Arg.setName(stmt.params[Idx++].value);
    }

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*context, "entry", f);
    builder->SetInsertPoint(bb);

    namedValues.clear();
    for (auto &Arg : f->args()) {
        llvm::AllocaInst* Alloca = builder->CreateAlloca(llvm::Type::getInt32Ty(*context), 0, Arg.getName());
        builder->CreateStore(&Arg, Alloca);
        namedValues[std::string(Arg.getName())] = Alloca;
    }

    for (const auto& statement : stmt.body) {
        statement->accept(*this);
    }

    llvm::verifyFunction(*f);
}

void CodeGenerator::visit(ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
}

void CodeGenerator::visit(IfStmt& stmt) {
    stmt.condition->accept(*this);
    llvm::Value* condValue = lastValue;

    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont");

    builder->CreateCondBr(condValue, thenBB, elseBB);

    builder->SetInsertPoint(thenBB);
    stmt.thenBranch->accept(*this);
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBB);
    }

    function->insert(function->end(), elseBB);
    builder->SetInsertPoint(elseBB);
    if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBB);
    }

    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);
}

void CodeGenerator::visit(BlockStmt& stmt) {
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
}
