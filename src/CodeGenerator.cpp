#include "CodeGenerator.h"
#include <stdexcept>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/Casting.h>
#include "SymbolTable.h"


CodeGenerator::CodeGenerator(SymbolTable& symbolTable, LifetimeManager& lifetimeManager)
    : typeResolver(symbolTable, lifetimeManager), isMemberAccess(false) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("ChthollyModule", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    declarePrintf();
    declareMalloc();
    declareStrcpy();
    declareFree();
}

void CodeGenerator::generate(BlockStmtAST& ast) {
    // First pass: declare all structs and functions
    for (auto& stmt : ast.statements) {
        if (auto* structDecl = dynamic_cast<StructDeclAST*>(stmt.get())) {
            visit(*structDecl);
        } else if (auto* classDecl = dynamic_cast<ClassDeclAST*>(stmt.get())) {
            visit(*classDecl);
        } else if (auto* enumDecl = dynamic_cast<EnumDeclAST*>(stmt.get())) {
            visit(*enumDecl);
        } else if (auto* funcDecl = dynamic_cast<FunctionDeclAST*>(stmt.get())) {
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
        if (!dynamic_cast<StructDeclAST*>(stmt.get()) && !dynamic_cast<ClassDeclAST*>(stmt.get())) {
            visit(*stmt);
        }
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
    if (auto* structType = dynamic_cast<const StructType*>(&type)) {
        return llvm::StructType::getTypeByName(*context, structType->name);
    }
    if (auto* classType = dynamic_cast<const ClassType*>(&type)) {
        return llvm::StructType::getTypeByName(*context, classType->name);
    }
    if (auto* arrayType = dynamic_cast<const ArrayType*>(&type)) {
        return llvm::ArrayType::get(resolveType(*arrayType->elementType), arrayType->size);
    }
    if (auto* dynamicArrayType = dynamic_cast<const DynamicArrayType*>(&type)) {
        auto* elementType = resolveType(*dynamicArrayType->elementType);
        auto* structType = llvm::StructType::getTypeByName(*context, "DynamicArray");
        if (!structType) {
            structType = llvm::StructType::create(*context, {elementType->getPointerTo(), builder->getInt64Ty(), builder->getInt64Ty()}, "DynamicArray");
        }
        return structType;
    }
    if (auto* enumType = dynamic_cast<const EnumType*>(&type)) {
        std::vector<llvm::Type*> body;
        body.push_back(builder->getInt32Ty()); // Tag

        uint64_t maxSize = 0;
        for (const auto& variant : enumType->variants) {
            uint64_t currentSize = 0;
            for (const auto& type : variant.second) {
                currentSize += module->getDataLayout().getTypeStoreSize(resolveType(*type));
            }
            if (currentSize > maxSize) {
                maxSize = currentSize;
            }
        }

        if (maxSize > 0) {
            body.push_back(llvm::ArrayType::get(builder->getInt8Ty(), maxSize));
        }

        auto* structType = llvm::StructType::getTypeByName(*context, enumType->name);
        if (!structType) {
            structType = llvm::StructType::create(*context, body, enumType->name);
        }
        return structType;
    }
    throw std::runtime_error("Unknown type in code generator");
}


llvm::Value* CodeGenerator::visit(ASTNode& node) {
    if (auto* p = dynamic_cast<VarDeclStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<StructDeclAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ClassDeclAST*>(&node)) {
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
    } else if (auto* p = dynamic_cast<StructInitializerExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<MemberAccessExprAST*>(&node)) {
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
    } else if (auto* p = dynamic_cast<ArrayLiteralExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ArrayIndexExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<EnumDeclAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<EnumVariantExprAST*>(&node)) {
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

llvm::Value* CodeGenerator::visit(ClassDeclAST& node) {
    std::vector<llvm::Type*> memberTypes;
    for (const auto& member : node.members) {
        memberTypes.push_back(resolveType(*member->type));
    }
    llvm::StructType::create(*context, memberTypes, node.name);

    for (const auto& method : node.methods) {
        std::string mangledName = "_ZN" + std::to_string(node.name.length()) + node.name + std::to_string(method->name.length()) + method->name + "Ev";
        llvm::Function* function = module->getFunction(mangledName);
        if (!function) {
            llvm::Type* returnType = resolveType(*method->returnType);
            std::vector<llvm::Type*> argTypes;
            // 'self' parameter
            argTypes.push_back(llvm::StructType::getTypeByName(*context, node.name)->getPointerTo());
            for (const auto& arg : method->args) {
                argTypes.push_back(resolveType(*arg.type));
            }
            llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, argTypes, false);
            function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, mangledName, module.get());
        }

        llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(*context, "entry", function);
        builder->SetInsertPoint(basicBlock);

        namedValues.clear();
        auto arg_it = function->arg_begin();
        arg_it->setName("self");
        llvm::AllocaInst* selfAlloca = builder->CreateAlloca(arg_it->getType(), nullptr, "self");
        builder->CreateStore(arg_it, selfAlloca);
        namedValues["self"] = selfAlloca;

        for (auto& arg : method->args) {
            ++arg_it;
            llvm::AllocaInst* alloca = builder->CreateAlloca(arg_it->getType(), nullptr, arg.name);
            builder->CreateStore(arg_it, alloca);
            namedValues[arg.name] = alloca;
        }

        visit(*method->body);
    }

    return nullptr;
}


llvm::Value* CodeGenerator::visit(StructDeclAST& node) {
    std::vector<llvm::Type*> memberTypes;
    for (const auto& member : node.members) {
        memberTypes.push_back(resolveType(*member->type));
    }
    llvm::StructType::create(*context, memberTypes, node.name);
    return nullptr;
}

llvm::Value* CodeGenerator::visit(StructInitializerExprAST& node) {
    auto* structType = llvm::StructType::getTypeByName(*context, node.structName);
    if (!structType) {
        throw std::runtime_error("Struct type '" + node.structName + "' not found.");
    }

    llvm::AllocaInst* alloca = builder->CreateAlloca(structType, nullptr, node.structName + ".instance");

    if (node.members[0]->name.empty()) { // Aggregate initialization
        for (size_t i = 0; i < node.members.size(); ++i) {
            llvm::Value* memberValue = visit(*node.members[i]->value);
            builder->CreateStore(memberValue, builder->CreateStructGEP(structType, alloca, i));
        }
    } else { // Designated initialization
        auto* structInfo = dynamic_cast<StructType*>(node.type.get());
        for (const auto& memberInit : node.members) {
            int memberIndex = -1;
            for (size_t i = 0; i < structInfo->members.size(); ++i) {
                if (structInfo->members[i].first == memberInit->name) {
                    memberIndex = i;
                    break;
                }
            }
            if (memberIndex == -1) {
                throw std::runtime_error("Struct '" + node.structName + "' has no member named '" + memberInit->name + "'.");
            }

            llvm::Value* memberValue = visit(*memberInit->value);
            builder->CreateStore(memberValue, builder->CreateStructGEP(structType, alloca, memberIndex));
        }
    }

    return alloca;
}

llvm::Value* CodeGenerator::visit(MemberAccessExprAST& node) {
    isMemberAccess = true;
    llvm::Value* object = visit(*node.object);
    isMemberAccess = false;
    auto* structType = dynamic_cast<StructType*>(node.object->type.get());
    if (!structType) {
        throw std::runtime_error("Member access on non-struct type.");
    }

    int memberIndex = -1;
    for (size_t i = 0; i < structType->members.size(); ++i) {
        if (structType->members[i].first == node.memberName) {
            memberIndex = i;
            break;
        }
    }

    if (memberIndex == -1) {
        throw std::runtime_error("Struct '" + structType->name + "' has no member named '" + node.memberName + "'.");
    }

    auto* llvmStructType = llvm::StructType::getTypeByName(*context, structType->name);
    llvm::Value* memberPtr = builder->CreateStructGEP(llvmStructType, object, memberIndex);
    return builder->CreateLoad(llvmStructType->getElementType(memberIndex), memberPtr);
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
    llvm::Type* varType = resolveType(*node.initExpr->type);
    llvm::AllocaInst* alloca = builder->CreateAlloca(varType, nullptr, node.varName);

    if (varType->isStructTy() && varType->getStructName() == "DynamicArray") {
        auto* staticArray = initVal;
        auto* staticArrayType = llvm::cast<llvm::ArrayType>(staticArray->getType()->getContainedType(0));
        auto* elementType = staticArrayType->getElementType();
        uint64_t numElements = staticArrayType->getNumElements();

        // Allocate memory on the heap
        auto* mallocFunc = module->getFunction("malloc");
        auto* size = builder->getInt64(numElements * module->getDataLayout().getTypeStoreSize(elementType));
        auto* data = builder->CreateCall(mallocFunc->getFunctionType(), mallocFunc, {size}, "data");

        // Copy elements from the static array to the heap
        builder->CreateMemCpy(data, llvm::MaybeAlign(), staticArray, llvm::MaybeAlign(), size);

        // Create the dynamic array struct
        builder->CreateStore(data, builder->CreateStructGEP(varType, alloca, 0));
        builder->CreateStore(builder->getInt64(numElements), builder->CreateStructGEP(varType, alloca, 1));
        builder->CreateStore(builder->getInt64(numElements), builder->CreateStructGEP(varType, alloca, 2)); // capacity = length for now
    } else if (varType->isStructTy() || varType->isArrayTy()) {
        builder->CreateMemCpy(alloca, llvm::MaybeAlign(), initVal, llvm::MaybeAlign(), module->getDataLayout().getTypeStoreSize(varType));
    } else {
        builder->CreateStore(initVal, alloca);
    }
    namedValues[node.varName] = alloca;

    if (node.initExpr->type->isString() || node.initExpr->type->isDynamicArray()) {
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
        auto* var = ownedValues[i];
        if (var->getAllocatedType()->isStructTy() && var->getAllocatedType()->getStructName() == "DynamicArray") {
            auto* arrayStruct = builder->CreateLoad(var->getAllocatedType(), var);
            auto* dataPtr = builder->CreateExtractValue(arrayStruct, {0});
            builder->CreateCall(freeFunc, {dataPtr});
        } else if (var->getAllocatedType()->isPointerTy()) { // String
            llvm::Value* val = builder->CreateLoad(var->getAllocatedType(), var);
            builder->CreateCall(freeFunc, {val});
        }
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

        if (rhs->getType()->isStructTy()) {
            builder->CreateMemCpy(lhs, llvm::MaybeAlign(), rhs, llvm::MaybeAlign(), module->getDataLayout().getTypeStoreSize(rhs->getType()));
        } else {
            builder->CreateStore(rhs, lhs);
        }

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
    if (isMemberAccess || v->getAllocatedType()->isArrayTy()) {
        return v;
    }
    return builder->CreateLoad(v->getAllocatedType(), v, node.name.c_str());
}

llvm::Value* CodeGenerator::visit(FunctionCallExprAST& node) {
    if (auto* memberAccess = dynamic_cast<MemberAccessExprAST*>(node.callee.get())) {
        auto* methodType = dynamic_cast<MethodType*>(memberAccess->type.get());
        std::string mangledName = "_ZN" + std::to_string(methodType->parentClass->name.length()) + methodType->parentClass->name + std::to_string(memberAccess->memberName.length()) + memberAccess->memberName + "Ev";
        llvm::Function* callee = module->getFunction(mangledName);
        if (!callee) {
            throw std::runtime_error("Unknown method referenced: " + memberAccess->memberName);
        }

        std::vector<llvm::Value*> argValues;
        argValues.push_back(visit(*memberAccess)); // self
        for (auto& arg : node.args) {
            argValues.push_back(visit(*arg));
        }

        return builder->CreateCall(callee, argValues, "calltmp");
    }

    llvm::Function* callee = nullptr;
    if (auto* var = dynamic_cast<VariableExprAST*>(node.callee.get())) {
        callee = module->getFunction(var->name);
        if (!callee) {
            // A temporary hack to support printf
            if (var->name == "println") {
                callee = module->getFunction("printf");
                if (!callee) throw std::runtime_error("Could not find printf function");
            } else {
                throw std::runtime_error("Unknown function referenced: " + var->name);
            }
        }
    } else if (auto* memberAccess = dynamic_cast<MemberAccessExprAST*>(node.callee.get())) {
        auto* methodType = dynamic_cast<MethodType*>(memberAccess->type.get());
        std::string mangledName = "_ZN" + std::to_string(methodType->parentClass->name.length()) + methodType->parentClass->name + std::to_string(memberAccess->memberName.length()) + memberAccess->memberName + "Ev";
        callee = module->getFunction(mangledName);
        if (!callee) {
            throw std::runtime_error("Unknown method referenced: " + memberAccess->memberName);
        }
    } else {
        throw std::runtime_error("Invalid callee expression.");
    }

    if (callee->arg_size() != node.args.size() && callee->isVarArg() == false) {
        if (auto* var = dynamic_cast<VariableExprAST*>(node.callee.get())) {
            throw std::runtime_error("Incorrect number of arguments passed to function " + var->name);
        } else {
            throw std::runtime_error("Incorrect number of arguments passed to function");
        }
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

llvm::Value* CodeGenerator::visit(ArrayLiteralExprAST& node) {
    auto* arrayType = llvm::cast<llvm::ArrayType>(resolveType(*node.type));
    auto* alloca = builder->CreateAlloca(arrayType, nullptr, "arraylit");

    for (size_t i = 0; i < node.elements.size(); ++i) {
        auto* elementValue = visit(*node.elements[i]);
        auto* gep = builder->CreateGEP(arrayType, alloca, {builder->getInt32(0), builder->getInt32(i)});
        builder->CreateStore(elementValue, gep);
    }

    return alloca;
}

llvm::Value* CodeGenerator::visit(ArrayIndexExprAST& node) {
    auto* array = visit(*node.array);
    auto* index = visit(*node.index);
    if (node.array->type->isDynamicArray()) {
        auto* dynArrayAlloca = namedValues[static_cast<VariableExprAST*>(node.array.get())->name];
        auto* dataPtrGEP = builder->CreateStructGEP(dynArrayAlloca->getAllocatedType(), dynArrayAlloca, 0);
        auto* dataPtr = builder->CreateLoad(resolveType(*static_cast<DynamicArrayType*>(node.array->type.get())->elementType)->getPointerTo(), dataPtrGEP);
        auto* gep = builder->CreateGEP(resolveType(*static_cast<DynamicArrayType*>(node.array->type.get())->elementType), dataPtr, index);
        return builder->CreateLoad(resolveType(*static_cast<DynamicArrayType*>(node.array->type.get())->elementType), gep);
    } else {
        auto* arrayType = llvm::cast<llvm::ArrayType>(llvm::cast<llvm::AllocaInst>(array)->getAllocatedType());
        auto* gep = builder->CreateGEP(arrayType, array, {builder->getInt32(0), index});
        return builder->CreateLoad(arrayType->getElementType(), gep);
    }
}

llvm::Value* CodeGenerator::visit(EnumDeclAST& node) {
    auto type = typeResolver.resolve(node);
    resolveType(*type);
    return nullptr;
}

llvm::Value* CodeGenerator::visit(EnumVariantExprAST& node) {
    auto* enumType = llvm::StructType::getTypeByName(*context, node.enumName);
    auto* enumAlloca = builder->CreateAlloca(enumType, nullptr, node.enumName);

    int variantIndex = 0;
    auto* enumInfo = static_cast<EnumType*>(node.type.get());
    for (auto it = enumInfo->variants.begin(); it != enumInfo->variants.end(); ++it) {
        if (it->first == node.variantName) {
            break;
        }
        variantIndex++;
    }

    builder->CreateStore(builder->getInt32(variantIndex), builder->CreateStructGEP(enumType, enumAlloca, 0));

    if (!node.args.empty()) {
        if (enumType->getNumElements() > 1) {
            auto* dataGEP = builder->CreateStructGEP(enumType, enumAlloca, 1);
            uint64_t offset = 0;
            for (size_t i = 0; i < node.args.size(); ++i) {
                auto* argValue = visit(*node.args[i]);
                auto* typedDataGEP = builder->CreateGEP(builder->getInt8Ty(), dataGEP, builder->getInt64(offset));
                builder->CreateStore(argValue, builder->CreateBitCast(typedDataGEP, argValue->getType()->getPointerTo()));
                offset += module->getDataLayout().getTypeStoreSize(argValue->getType());
            }
        }
    }

    return builder->CreateLoad(enumType, enumAlloca);
}
