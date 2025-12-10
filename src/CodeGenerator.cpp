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
    declareMalloc();
    declareStrcpy();
    declareFree();
}

void CodeGenerator::generate(BlockStmtAST& ast) {
    // First pass: declare all functions
    for (auto& stmt : ast.statements) {
        if (auto* funcDecl = dynamic_cast<FunctionDeclAST*>(stmt.get())) {
            llvm::Type* returnType = resolveType(*funcDecl->returnType);
            std::vector<llvm::Type*> argTypes;
            for (const auto& arg : funcDecl->args) {
                argTypes.push_back(resolveType(*arg.type));
            }
            llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, argTypes, false);
            llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, funcDecl->name, module.get());
        }
    }

    // Second pass: generate code for all statements
    for (auto& stmt : ast.statements) {
        visit(*stmt);
    }
}

void CodeGenerator::dump() const {
    module->print(llvm::errs(), nullptr);
}

void CodeGenerator::declarePrintf() {
    llvm::Type* i8_ptr_type = builder->getInt8Ty()->getPointerTo();
    llvm::FunctionType* printf_type = llvm::FunctionType::get(builder->getInt32Ty(), {i8_ptr_type}, true);
    module->getOrInsertFunction("printf", printf_type);
}

void CodeGenerator::declareMalloc() {
    llvm::Type* i8_ptr_type = builder->getInt8Ty()->getPointerTo();
    llvm::FunctionType* malloc_type = llvm::FunctionType::get(i8_ptr_type, {builder->getInt64Ty()}, false);
    module->getOrInsertFunction("malloc", malloc_type);
}

void CodeGenerator::declareStrcpy() {
    llvm::Type* i8_ptr_type = builder->getInt8Ty()->getPointerTo();
    llvm::FunctionType* strcpy_type = llvm::FunctionType::get(i8_ptr_type, {i8_ptr_type, i8_ptr_type}, false);
    module->getOrInsertFunction("strcpy", strcpy_type);
}

void CodeGenerator::declareFree() {
    llvm::Type* i8_ptr_type = builder->getInt8Ty()->getPointerTo();
    llvm::FunctionType* free_type = llvm::FunctionType::get(builder->getVoidTy(), {i8_ptr_type}, false);
    module->getOrInsertFunction("free", free_type);
}

void CodeGenerator::createStringSwitch(SwitchStmtAST& node) {
    llvm::Value* condV = visit(*node.condition);
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* switchEndBB = llvm::BasicBlock::Create(*context, "switch.end", function);

    llvm::BasicBlock* oldSwitchExit = currentSwitchExit;
    currentSwitchExit = switchEndBB;

    llvm::BasicBlock* nextCaseBB = llvm::BasicBlock::Create(*context, "switch.case.check", function);
    builder->CreateBr(nextCaseBB);

    llvm::Function* strcmpFunc = module->getFunction("strcmp");
    if (!strcmpFunc) {
        llvm::Type* i8_ptr_type = builder->getInt8Ty()->getPointerTo();
        llvm::FunctionType* strcmp_type = llvm::FunctionType::get(builder->getInt32Ty(), {i8_ptr_type, i8_ptr_type}, false);
        strcmpFunc = llvm::cast<llvm::Function>(module->getOrInsertFunction("strcmp", strcmp_type).getCallee());
    }

    llvm::BasicBlock* defaultBB = nullptr;

    for (const auto& caseBlock : node.cases) {
        if (caseBlock->value) {
            builder->SetInsertPoint(nextCaseBB);
            llvm::Value* caseVal = visit(*caseBlock->value);
            llvm::Value* cmp = builder->CreateCall(strcmpFunc, {condV, caseVal}, "strcmp");
            llvm::Value* is_eq = builder->CreateICmpEQ(cmp, builder->getInt32(0), "is_eq");

            llvm::BasicBlock* caseBB = llvm::BasicBlock::Create(*context, "switch.case", function);
            nextCaseBB = llvm::BasicBlock::Create(*context, "switch.case.check", function);
            builder->CreateCondBr(is_eq, caseBB, nextCaseBB);

            builder->SetInsertPoint(caseBB);
            visit(*caseBlock->body);
            if (!builder->GetInsertBlock()->getTerminator()) {
                builder->CreateBr(switchEndBB);
            }
        } else {
            defaultBB = llvm::BasicBlock::Create(*context, "switch.default", function);
            builder->SetInsertPoint(defaultBB);
            visit(*caseBlock->body);
            if (!builder->GetInsertBlock()->getTerminator()) {
                builder->CreateBr(switchEndBB);
            }
        }
    }

    builder->SetInsertPoint(nextCaseBB);
    if (defaultBB) {
        builder->CreateBr(defaultBB);
    } else {
        builder->CreateBr(switchEndBB);
    }

    builder->SetInsertPoint(switchEndBB);
    currentSwitchExit = oldSwitchExit;
}


llvm::Type* CodeGenerator::resolveType(const TypeNameAST& typeName) {
    if (auto* refType = dynamic_cast<const ReferenceTypeAST*>(&typeName)) {
        return resolveType(*refType->referencedType)->getPointerTo();
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

llvm::Type* CodeGenerator::resolveType(const Type& type) {
    if (auto* intType = dynamic_cast<const IntegerType*>(&type)) {
        return builder->getInt32Ty();
    }
    if (auto* floatType = dynamic_cast<const FloatType*>(&type)) {
        return builder->getDoubleTy();
    }
    if (auto* stringType = dynamic_cast<const StringType*>(&type)) {
        return builder->getInt8Ty()->getPointerTo();
    }
    if (auto* boolType = dynamic_cast<const BoolType*>(&type)) {
        return builder->getInt1Ty();
    }
    if (auto* voidType = dynamic_cast<const VoidType*>(&type)) {
        return builder->getVoidTy();
    }
    if (auto* refType = dynamic_cast<const ReferenceType*>(&type)) {
        return resolveType(*refType->referencedType)->getPointerTo();
    }
    throw std::runtime_error("Unknown type in code generator");
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
    } else if (auto* p = dynamic_cast<DoWhileStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ForStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<SwitchStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BreakStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<FallthroughStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BorrowExprAST*>(&node)) {
        return visit(*p);
    }
    return nullptr;
}

llvm::Value* CodeGenerator::visit(SwitchStmtAST& node) {
    if (node.condition->type->isString()) {
        createStringSwitch(node);
        return nullptr;
    }

    llvm::Value* condV = visit(*node.condition);
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* switchEndBB = llvm::BasicBlock::Create(*context, "switch.end", function);

    // Save the old switch exit and set the new one
    llvm::BasicBlock* oldSwitchExit = currentSwitchExit;
    currentSwitchExit = switchEndBB;

    llvm::SwitchInst* switchInst = builder->CreateSwitch(condV, switchEndBB, node.cases.size());

    for (const auto& caseBlock : node.cases) {
        if (caseBlock->value) {
            llvm::ConstantInt* caseVal = llvm::cast<llvm::ConstantInt>(visit(*caseBlock->value));
            llvm::BasicBlock* caseBB = llvm::BasicBlock::Create(*context, "switch.case", function);
            switchInst->addCase(caseVal, caseBB);
            builder->SetInsertPoint(caseBB);
        } else {
            llvm::BasicBlock* defaultBB = llvm::BasicBlock::Create(*context, "switch.default", function);
            switchInst->setDefaultDest(defaultBB);
            builder->SetInsertPoint(defaultBB);
        }

        visit(*caseBlock->body);

        // Check for fallthrough or break
        bool hasTerminator = !caseBlock->body->statements.empty() &&
                             (dynamic_cast<FallthroughStmtAST*>(caseBlock->body->statements.back().get()) ||
                              dynamic_cast<BreakStmtAST*>(caseBlock->body->statements.back().get()));

        if (!builder->GetInsertBlock()->getTerminator() && !hasTerminator) {
            builder->CreateBr(switchEndBB);
        }
    }

    builder->SetInsertPoint(switchEndBB);

    // Restore the old switch exit
    currentSwitchExit = oldSwitchExit;

    return nullptr;
}


llvm::Value* CodeGenerator::visit(BreakStmtAST& node) {
    if (!currentSwitchExit) {
        throw std::runtime_error("Break statement outside of a switch statement.");
    }
    builder->CreateBr(currentSwitchExit);
    return nullptr;
}

llvm::Value* CodeGenerator::visit(FallthroughStmtAST& node) {
    // A fallthrough is implemented by not creating a branch to the switch.end
    // block. The IR builder will continue to the next basic block, which will
    // be the next case.
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

    if (node.initExpr->type->isString()) {
        if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.initExpr.get())) {
            // Find the moved variable in ownedValues and remove it
            llvm::AllocaInst* movedAlloca = namedValues[varExpr->name];
            for (auto it = ownedValues.begin(); it != ownedValues.end(); ++it) {
                if (*it == movedAlloca) {
                    ownedValues.erase(it);
                    break;
                }
            }
        }
        ownedValues.push_back(alloca);
    }

    return alloca;
}

llvm::Value* CodeGenerator::visit(FunctionDeclAST& node) {
    llvm::Function* function = module->getFunction(node.name);
    if (!function) {
        throw std::runtime_error("Function '" + node.name + "' not declared in code generator.");
    }

    llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(basicBlock);

    namedValues.clear(); // Clear old variables
    unsigned i = 0;
    for (auto& arg : function->args()) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(arg.getType(), nullptr, node.args[i].name);
        builder->CreateStore(&arg, alloca);
        namedValues[node.args[i].name] = alloca;
        if (node.args[i].resolvedType->isString()) {
            ownedValues.push_back(alloca);
        }
        i++;
    }

    visit(*node.body);

    return function;
}

llvm::Value* CodeGenerator::visit(BlockStmtAST& node) {
    size_t ownedValuesInScope = ownedValues.size();
    for (auto& stmt : node.statements) {
        if (stmt) {
            visit(*stmt);
        }
    }

    llvm::Function* freeFunc = module->getFunction("free");
    for (size_t i = ownedValuesInScope; i < ownedValues.size(); ++i) {
        llvm::Value* val = builder->CreateLoad(ownedValues[i]->getAllocatedType(), ownedValues[i]);
        builder->CreateCall(freeFunc, {val});
    }
    ownedValues.resize(ownedValuesInScope);

    return nullptr;
}

llvm::Value* CodeGenerator::visit(BinaryExprAST& node) {
    if (node.op == TokenType::EQUAL) {
        auto* lhsVar = dynamic_cast<VariableExprAST*>(node.lhs.get());
        if (!lhsVar) {
            throw std::runtime_error("The left-hand side of an assignment must be a variable.");
        }
        llvm::AllocaInst* lhs = namedValues[lhsVar->name];
        if (!lhs) {
            throw std::runtime_error("Unknown variable name: " + lhsVar->name);
        }

        llvm::Value* rhs = visit(*node.rhs);

        if (node.rhs->type->isString()) {
            llvm::Function* freeFunc = module->getFunction("free");
            llvm::Value* oldVal = builder->CreateLoad(lhs->getAllocatedType(), lhs);
            builder->CreateCall(freeFunc, {oldVal});
        }

        builder->CreateStore(rhs, lhs);

        if (node.rhs->type->isString()) {
             if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.rhs.get())) {
                // Find the moved variable in ownedValues and remove it
                llvm::AllocaInst* movedAlloca = namedValues[varExpr->name];
                for (auto it = ownedValues.begin(); it != ownedValues.end(); ++it) {
                    if (*it == movedAlloca) {
                        ownedValues.erase(it);
                        break;
                    }
                }
            }
            ownedValues.push_back(lhs);
        }

        return rhs;
    }

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
    llvm::Function* mallocFunc = module->getFunction("malloc");
    llvm::Function* strcpyFunc = module->getFunction("strcpy");

    llvm::Value* strSize = builder->getInt64(node.value.size() + 1);
    llvm::Value* mem = builder->CreateCall(mallocFunc, {strSize}, "malloccall");
    llvm::Value* globalStr = builder->CreateGlobalStringPtr(node.value);
    builder->CreateCall(strcpyFunc, {mem, globalStr});
    return mem;
}

llvm::Value* CodeGenerator::visit(BoolExprAST& node) {
    return llvm::ConstantInt::get(builder->getInt1Ty(), node.value);
}

llvm::Value* CodeGenerator::visit(VariableExprAST& node) {
    llvm::AllocaInst* v = namedValues[node.name];
    if (!v) {
        throw std::runtime_error("Unknown variable name: " + node.name);
    }

    if (auto* refType = dynamic_cast<ReferenceType*>(node.type.get())) {
        llvm::Value* ptr = builder->CreateLoad(v->getAllocatedType(), v);
        return builder->CreateLoad(resolveType(*refType->referencedType), ptr);
    }

    // Load the value from the memory location
    return builder->CreateLoad(v->getAllocatedType(), v, node.name.c_str());
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
        if (arg->type->isString()) {
            if (auto* varExpr = dynamic_cast<VariableExprAST*>(arg.get())) {
                // Find the moved variable in ownedValues and remove it
                llvm::AllocaInst* movedAlloca = namedValues[varExpr->name];
                for (auto it = ownedValues.begin(); it != ownedValues.end(); ++it) {
                    if (*it == movedAlloca) {
                        ownedValues.erase(it);
                        break;
                    }
                }
            }
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

llvm::Value* CodeGenerator::visit(WhileStmtAST& node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* loopHeader = llvm::BasicBlock::Create(*context, "loop.header", function);
    llvm::BasicBlock* loopBody = llvm::BasicBlock::Create(*context, "loop.body", function);
    llvm::BasicBlock* loopExit = llvm::BasicBlock::Create(*context, "loop.exit", function);

    builder->CreateBr(loopHeader);
    builder->SetInsertPoint(loopHeader);

    llvm::Value* condV = visit(*node.condition);
    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(builder->getInt1Ty(), 0), "whilecond");
    builder->CreateCondBr(condV, loopBody, loopExit);

    builder->SetInsertPoint(loopBody);
    visit(*node.body);
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(loopHeader);
    }

    builder->SetInsertPoint(loopExit);

    return nullptr;
}

llvm::Value* CodeGenerator::visit(DoWhileStmtAST& node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* loopBody = llvm::BasicBlock::Create(*context, "loop.body", function);
    llvm::BasicBlock* loopCond = llvm::BasicBlock::Create(*context, "loop.cond", function);
    llvm::BasicBlock* loopExit = llvm::BasicBlock::Create(*context, "loop.exit", function);

    builder->CreateBr(loopBody);
    builder->SetInsertPoint(loopBody);

    visit(*node.body);
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(loopCond);
    }

    builder->SetInsertPoint(loopCond);
    llvm::Value* condV = visit(*node.condition);
    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(builder->getInt1Ty(), 0), "dowhilecond");
    builder->CreateCondBr(condV, loopBody, loopExit);

    builder->SetInsertPoint(loopExit);

    return nullptr;
}

llvm::Value* CodeGenerator::visit(ForStmtAST& node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    // Create blocks for the loop
    llvm::BasicBlock* initBB = llvm::BasicBlock::Create(*context, "for.init", function);
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "for.cond", function);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "for.body", function);
    llvm::BasicBlock* incBB = llvm::BasicBlock::Create(*context, "for.inc", function);
    llvm::BasicBlock* exitBB = llvm::BasicBlock::Create(*context, "for.exit", function);

    builder->CreateBr(initBB);
    builder->SetInsertPoint(initBB);
    if (node.init) {
        visit(*node.init);
    }
    builder->CreateBr(condBB);

    builder->SetInsertPoint(condBB);
    if (node.condition) {
        llvm::Value* condV = visit(*node.condition);
        condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(builder->getInt1Ty(), 0), "forcond");
        builder->CreateCondBr(condV, bodyBB, exitBB);
    } else {
        builder->CreateBr(bodyBB); // No condition means infinite loop
    }

    builder->SetInsertPoint(bodyBB);
    visit(*node.body);
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(incBB);
    }

    builder->SetInsertPoint(incBB);
    if (node.increment) {
        visit(*node.increment);
    }
    builder->CreateBr(condBB);

    builder->SetInsertPoint(exitBB);

    return nullptr;
}


llvm::Value* CodeGenerator::visit(BorrowExprAST& node) {
    if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.expression.get())) {
        return namedValues[varExpr->name];
    }
    throw std::runtime_error("Can only borrow variables for now.");
}
