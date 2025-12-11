#include "SemanticAnalyzer.h"
#include <iostream>
#include <stdexcept>
#include <utility>
#include "Type.h"

SemanticAnalyzer::SemanticAnalyzer() : typeResolver(symbolTable) {
    // Pre-populate with built-in functions
    auto printlnType = std::make_shared<FunctionType>(); // This is a simplification
    symbolTable.addSymbol("println", printlnType, false);
}

void SemanticAnalyzer::analyze(BlockStmtAST& ast) {
    // First pass: register all type and function declarations
    for (auto& stmt : ast.statements) {
        if (auto* structDecl = dynamic_cast<StructDeclAST*>(stmt.get())) {
            visit(*structDecl);
        } else if (auto* classDecl = dynamic_cast<ClassDeclAST*>(stmt.get())) {
            visit(*classDecl);
        } else if (auto* enumDecl = dynamic_cast<EnumDeclAST*>(stmt.get())) {
            visit(*enumDecl);
        } else if (auto* funcDecl = dynamic_cast<FunctionDeclAST*>(stmt.get())) {
            auto funcType = std::make_shared<FunctionType>();
            funcType->returnType = typeResolver.resolve(*funcDecl->returnType);
            for (auto& arg : funcDecl->args) {
                arg.resolvedType = typeResolver.resolve(*arg.type);
                funcType->argTypes.push_back(arg.resolvedType);
            }

            if (!symbolTable.addSymbol(funcDecl->name, funcType, false)) {
                throw std::runtime_error("Function '" + funcDecl->name + "' already declared in this scope.");
            }
        }
    }

    // Second pass: analyze all statements
    for (auto& stmt : ast.statements) {
        if (!dynamic_cast<StructDeclAST*>(stmt.get()) && !dynamic_cast<ClassDeclAST*>(stmt.get())) {
            visit(*stmt);
        }
    }
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ASTNode& node) {
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
    } else if (auto* p = dynamic_cast<CaseBlockAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BreakStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<FallthroughStmtAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<TypeNameAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<BorrowExprAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ReferenceTypeAST*>(&node)) {
        return visit(*p);
    } else if (auto* p = dynamic_cast<ArrayTypeAST*>(&node)) {
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

std::shared_ptr<Type> SemanticAnalyzer::visit(VarDeclStmtAST& node) {
    std::shared_ptr<Type> initType = nullptr;
    if (node.initExpr) {
        initType = visit(*node.initExpr);
        if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.initExpr.get())) {
            Symbol* symbol = symbolTable.findSymbol(varExpr->name);
            if (symbol && !symbol->type->isCopy()) {
                symbol->isMoved = true;
            }
        }
    }

    std::shared_ptr<Type> declaredType = nullptr;
    if (node.type) {
        declaredType = typeResolver.resolve(*node.type);
    }

    if (declaredType && initType) {
        if (declaredType->isDynamicArray() && initType->isArray()) {
            auto* dynamicArrayType = static_cast<DynamicArrayType*>(declaredType.get());
            auto* staticArrayType = static_cast<ArrayType*>(initType.get());
            if (dynamicArrayType->elementType->toString() != staticArrayType->elementType->toString()) {
                throw std::runtime_error("Type mismatch for variable '" + node.varName + "'. Expected dynamic array of " + dynamicArrayType->elementType->toString() + " but got static array of " + staticArrayType->elementType->toString());
            }
        } else if (declaredType->toString() != initType->toString()) {
            throw std::runtime_error("Type mismatch for variable '" + node.varName + "'. Expected " + declaredType->toString() + " but got " + initType->toString());
        }
    }

    auto varType = declaredType ? declaredType : initType;
    if (!varType) {
        throw std::runtime_error("Variable '" + node.varName + "' must have a type annotation or an initializer.");
    }

    if (!symbolTable.addSymbol(node.varName, varType, node.isMutable)) {
        throw std::runtime_error("Variable '" + node.varName + "' already declared in this scope.");
    }
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ClassDeclAST& node) {
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> members;
    for (const auto& member : node.members) {
        auto memberType = typeResolver.resolve(*member->type);
        if (member->defaultValue) {
            auto defaultType = visit(*member->defaultValue);
            if (memberType->toString() != defaultType->toString()) {
                throw std::runtime_error("Type mismatch for default value of member '" + member->name + "' in class '" + node.name + "'.");
            }
        }
        members.emplace_back(member->name, memberType);
    }

    std::unordered_map<std::string, std::shared_ptr<FunctionType>> methods;
    for (const auto& method : node.methods) {
        auto funcType = std::make_shared<FunctionType>();
        funcType->returnType = typeResolver.resolve(*method->returnType);
        for (auto& arg : method->args) {
            arg.resolvedType = typeResolver.resolve(*arg.type);
            funcType->argTypes.push_back(arg.resolvedType);
        }
        methods[method->name] = funcType;
    }

    auto classType = std::make_shared<ClassType>(node.name, members, methods);
    if (!symbolTable.add_type(node.name, classType)) {
        throw std::runtime_error("Class '" + node.name + "' already declared.");
    }

    // Analyze method bodies
    symbolTable.enterScope();
    symbolTable.add_type("Self", classType);
    for (const auto& method : node.methods) {
        visit(*method);
    }
    symbolTable.leaveScope();

    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(StructDeclAST& node) {
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> members;
    for (const auto& member : node.members) {
        auto memberType = typeResolver.resolve(*member->type);
        if (member->defaultValue) {
            auto defaultType = visit(*member->defaultValue);
            if (memberType->toString() != defaultType->toString()) {
                throw std::runtime_error("Type mismatch for default value of member '" + member->name + "' in struct '" + node.name + "'.");
            }
        }
        members.emplace_back(member->name, memberType);
    }
    auto structType = std::make_shared<StructType>(node.name, members);
    if (!symbolTable.add_type(node.name, structType)) {
        throw std::runtime_error("Struct '" + node.name + "' already declared.");
    }
    return nullptr; // Statements don't have a type
}


std::shared_ptr<Type> SemanticAnalyzer::visit(FunctionDeclAST& node) {
    auto returnType = typeResolver.resolve(*node.returnType);
    currentFunction = &node;
    currentFunctionLifetime = symbolTable.getCurrentLifetime();
    symbolTable.enterScope();
    for (auto& arg : node.args) {
        if(!symbolTable.addSymbol(arg.name, arg.resolvedType, false)) {
             throw std::runtime_error("Argument '" + arg.name + "' already declared in this scope.");
        }
    }
    visit(*node.body);

    if (returnType->toString() != "void") {
        bool hasReturn = false;
        if(node.body && !node.body->statements.empty()) {
            if(dynamic_cast<ReturnStmtAST*>(node.body->statements.back().get())) {
                hasReturn = true;
            }
        }

        if (!hasReturn) {
            throw std::runtime_error("Non-void function '" + node.name + "' must have a return statement as the last statement.");
        }
    }


    symbolTable.leaveScope();
    currentFunction = nullptr;
    currentFunctionLifetime = -1;
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BlockStmtAST& node) {
    symbolTable.enterScope();
    for (auto& stmt : node.statements) {
        if(stmt) visit(*stmt);
    }

    for (auto& pair : symbolTable.getCurrentScope()) {
        Symbol& symbol = pair.second;
        if (symbol.borrowedInScope) {
            symbol.immutableBorrows = 0;
            symbol.mutableBorrow = false;
            symbol.borrowedInScope = false;
        }
    }

    symbolTable.leaveScope();
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BinaryExprAST& node) {
    if (node.op == TokenType::EQUAL) {
        auto* lhsVar = dynamic_cast<VariableExprAST*>(node.lhs.get());
        if (!lhsVar) {
            throw std::runtime_error("The left-hand side of an assignment must be a variable.");
        }

        Symbol* lhsSymbol = symbolTable.findSymbol(lhsVar->name);
        if (!lhsSymbol) {
            throw std::runtime_error("Variable '" + lhsVar->name + "' not declared.");
        }
        if (!lhsSymbol->isMutable) {
            throw std::runtime_error("Variable '" + lhsVar->name + "' is not mutable.");
        }

        auto rhsType = visit(*node.rhs);
        if (auto* rhsVar = dynamic_cast<VariableExprAST*>(node.rhs.get())) {
            Symbol* rhsSymbol = symbolTable.findSymbol(rhsVar->name);
            if (rhsSymbol && !rhsSymbol->type->isCopy()) {
                rhsSymbol->isMoved = true;
            }
        }

        if (lhsSymbol->type->toString() != rhsType->toString()) {
            throw std::runtime_error("Type mismatch in assignment to variable '" + lhsVar->name + "'.");
        }
        node.type = lhsSymbol->type;
        return node.type;
    }


    auto lhsType = visit(*node.lhs);
    auto rhsType = visit(*node.rhs);

    if (!lhsType || !rhsType) {
        throw std::runtime_error("Could not resolve type for one or both operands in binary expression.");
    }

    if (node.op == TokenType::LESS || node.op == TokenType::LESS_EQUAL || node.op == TokenType::GREATER || node.op == TokenType::GREATER_EQUAL || node.op == TokenType::DOUBLE_EQUAL || node.op == TokenType::NOT_EQUAL) {
        if (lhsType->toString() != rhsType->toString()) {
            throw std::runtime_error("Type mismatch in binary expression: " + lhsType->toString() + " vs " + rhsType->toString());
        }
        node.type = std::make_shared<BoolType>();
        return node.type;
    }

    if ((!lhsType->isInteger() && !lhsType->isFloat()) || (!rhsType->isInteger() && !rhsType->isFloat())) {
        throw std::runtime_error("Binary operator applied to non-numeric type.");
    }

    if (lhsType->toString() != rhsType->toString()) {
        throw std::runtime_error("Type mismatch in binary expression: " + lhsType->toString() + " vs " + rhsType->toString());
    }

    node.type = lhsType;
    return lhsType;
}

#include <cmath>

std::shared_ptr<Type> SemanticAnalyzer::visit(NumberExprAST& node) {
    double intpart;
    if (modf(node.value, &intpart) == 0.0) {
        node.type = std::make_shared<IntegerType>(32, true);
    } else {
        node.type = std::make_shared<FloatType>(64);
    }
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(EnumDeclAST& node) {
    std::unordered_map<std::string, std::vector<std::shared_ptr<Type>>> variants;
    for (const auto& variant : node.variants) {
        std::vector<std::shared_ptr<Type>> types;
        for (const auto& type : variant->types) {
            types.push_back(typeResolver.resolve(*type));
        }
        variants[variant->name] = types;
    }
    auto enumType = std::make_shared<EnumType>(node.name, variants);
    if (!symbolTable.add_type(node.name, enumType)) {
        throw std::runtime_error("Enum '" + node.name + "' already declared.");
    }
    return nullptr;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(EnumVariantExprAST& node) {
    auto type = symbolTable.find_type(node.enumName);
    if (!type || !type->isEnum()) {
        throw std::runtime_error("Type '" + node.enumName + "' is not an enum.");
    }
    auto* enumType = static_cast<EnumType*>(type.get());
    if (enumType->variants.find(node.variantName) == enumType->variants.end()) {
        throw std::runtime_error("Enum '" + node.enumName + "' has no variant named '" + node.variantName + "'.");
    }
    auto& variantTypes = enumType->variants[node.variantName];
    if (variantTypes.size() != node.args.size()) {
        throw std::runtime_error("Incorrect number of arguments for enum variant '" + node.variantName + "'.");
    }
    for (size_t i = 0; i < node.args.size(); ++i) {
        auto argType = visit(*node.args[i]);
        if (argType->toString() != variantTypes[i]->toString()) {
            throw std::runtime_error("Type mismatch for argument " + std::to_string(i) + " of enum variant '" + node.variantName + "'.");
        }
    }
    node.type = type;
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(StringExprAST& node) {
    node.type = std::make_shared<StringType>();
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BoolExprAST& node) {
    node.type = std::make_shared<BoolType>();
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(VariableExprAST& node) {
    Symbol* symbol = symbolTable.findSymbol(node.name);
    if (!symbol) {
        throw std::runtime_error("Variable '" + node.name + "' not declared.");
    }
    if (symbol->isMoved) {
        throw std::runtime_error("Variable '" + node.name + "' has been moved and is no longer valid.");
    }
    node.type = symbol->type;
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(StructInitializerExprAST& node) {
    auto type = symbolTable.find_type(node.structName);
    if (!type) {
        throw std::runtime_error("Type '" + node.structName + "' not declared.");
    }

    if (auto* structType = dynamic_cast<StructType*>(type.get())) {
        if (node.members.size() != structType->members.size()) {
            throw std::runtime_error("Incorrect number of initializers for struct '" + node.structName + "'.");
        }
        // ... (rest of the logic for structs)
    } else if (auto* classType = dynamic_cast<ClassType*>(type.get())) {
        if (node.members.size() != classType->members.size()) {
            throw std::runtime_error("Incorrect number of initializers for class '" + node.structName + "'.");
        }
        // ... (logic for classes)
    } else {
        throw std::runtime_error("'" + node.structName + "' is not a struct or class.");
    }

    // Common logic for both structs and classes
    // ...

    node.type = type;
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(MemberAccessExprAST& node) {
    auto objectType = visit(*node.object);
    if (auto* structType = dynamic_cast<StructType*>(objectType.get())) {
        for (const auto& member : structType->members) {
            if (member.first == node.memberName) {
                node.type = member.second;
                return node.type;
            }
        }
    } else if (auto* classType = dynamic_cast<ClassType*>(objectType.get())) {
        for (const auto& member : classType->members) {
            if (member.first == node.memberName) {
                node.type = member.second;
                return node.type;
            }
        }
        if (classType->methods.count(node.memberName)) {
            node.type = std::make_shared<MethodType>(std::dynamic_pointer_cast<ClassType>(objectType), classType->methods[node.memberName]);
            return node.type;
        }
    } else {
        throw std::runtime_error("Member access on non-struct or non-class type.");
    }

    throw std::runtime_error("Type '" + objectType->toString() + "' has no member named '" + node.memberName + "'.");
}


std::shared_ptr<Type> SemanticAnalyzer::visit(FunctionCallExprAST& node) {
    // Handle println separately
    if (auto* var = dynamic_cast<VariableExprAST*>(node.callee.get())) {
        if (var->name == "println") {
            for (auto& arg : node.args) {
                visit(*arg);
                if (auto* varExpr = dynamic_cast<VariableExprAST*>(arg.get())) {
                    Symbol* argSymbol = symbolTable.findSymbol(varExpr->name);
                    if (argSymbol && argSymbol->isMoved) {
                        throw std::runtime_error("Variable '" + varExpr->name + "' has been moved and is no longer valid.");
                    }
                    // Do not move the argument for println
                }
            }
            return std::make_shared<VoidType>();
        }
    }

    // Resolve the callee type
    auto calleeType = visit(*node.callee);
    if (!calleeType) {
        throw std::runtime_error("Could not resolve callee type for function call.");
    }

    // Get the function type
    FunctionType* funcType = nullptr;
    if (auto* ft = dynamic_cast<FunctionType*>(calleeType.get())) {
        funcType = ft;
    } else if (auto* mt = dynamic_cast<MethodType*>(calleeType.get())) {
        funcType = mt;
    } else {
        throw std::runtime_error("Expression is not a function, cannot be called.");
    }

    // Check arguments
    for (auto& arg : node.args) {
        visit(*arg);
        if (auto* varExpr = dynamic_cast<VariableExprAST*>(arg.get())) {
            Symbol* argSymbol = symbolTable.findSymbol(varExpr->name);
            if (argSymbol && !argSymbol->type->isCopy()) {
                argSymbol->isMoved = true;
            }
        }
    }

    node.type = funcType->returnType;
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ExprStmtAST& node) {
    visit(*node.expr);
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ReturnStmtAST& node) {
    if (!currentFunction) {
        throw std::runtime_error("Return statement outside of a function.");
    }
    if (node.returnValue) {
        auto returnType = visit(*node.returnValue);
        auto expectedType = typeResolver.resolve(*currentFunction->returnType);
        if (returnType->toString() != expectedType->toString()) {
            throw std::runtime_error("Return type mismatch: expected " + expectedType->toString() + ", got " + returnType->toString());
        }

        if (auto* borrowExpr = dynamic_cast<BorrowExprAST*>(node.returnValue.get())) {
            if (auto* varExpr = dynamic_cast<VariableExprAST*>(borrowExpr->expression.get())) {
                Symbol* symbol = symbolTable.findSymbol(varExpr->name);
                if (symbol->lifetime > currentFunctionLifetime) {
                    throw std::runtime_error("Cannot return a reference to a local variable '" + varExpr->name + "'.");
                }
            }
        }
    } else {
        auto expectedType = typeResolver.resolve(*currentFunction->returnType);
        if (expectedType->toString() != "void") {
            throw std::runtime_error("Non-void function must return a value.");
        }
    }
    return nullptr;
}


std::shared_ptr<Type> SemanticAnalyzer::visit(IfStmtAST& node) {
    auto conditionType = visit(*node.condition);
    if (!conditionType->isBool()) {
        throw std::runtime_error("If condition must be a boolean expression.");
    }

    visit(*node.thenBranch);
    if (node.elseBranch) {
        visit(*node.elseBranch);
    }

    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(SwitchStmtAST& node) {
    auto conditionType = visit(*node.condition);
    if (!conditionType->isInteger() && !conditionType->isString()) {
        throw std::runtime_error("Switch condition must be an integer or a string.");
    }

    bool oldInSwitch = inSwitch;
    inSwitch = true;

    for (auto& caseBlock : node.cases) {
        visit(*caseBlock);
        if (caseBlock->value) {
            if (!dynamic_cast<NumberExprAST*>(caseBlock->value.get()) && !dynamic_cast<StringExprAST*>(caseBlock->value.get())) {
                throw std::runtime_error("Case value must be a literal.");
            }
            auto caseType = visit(*caseBlock->value);
            if (caseType->toString() != conditionType->toString()) {
                throw std::runtime_error("Case value type does not match switch condition type.");
            }
        }
    }

    inSwitch = oldInSwitch;

    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(CaseBlockAST& node) {
    if (node.value) {
        visit(*node.value);
    }
    visit(*node.body);
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BreakStmtAST& node) {
    if (!inSwitch) {
        throw std::runtime_error("Break statement outside of a switch statement.");
    }
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(FallthroughStmtAST& node) {
    if (!inSwitch) {
        throw std::runtime_error("Fallthrough statement outside of a switch statement.");
    }
    return nullptr; // Statements don't have a type
}

std::shared_ptr<Type> SemanticAnalyzer::visit(WhileStmtAST& node) {
    auto conditionType = visit(*node.condition);
    if (!conditionType->isBool()) {
        throw std::runtime_error("While condition must be a boolean expression.");
    }
    visit(*node.body);
    return nullptr;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(DoWhileStmtAST& node) {
    visit(*node.body);
    auto conditionType = visit(*node.condition);
    if (!conditionType->isBool()) {
        throw std::runtime_error("Do-while condition must be a boolean expression.");
    }
    return nullptr;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ForStmtAST& node) {
    symbolTable.enterScope();
    if (node.init) {
        visit(*node.init);
    }
    if (node.condition) {
        auto conditionType = visit(*node.condition);
        if (!conditionType->isBool()) {
            throw std::runtime_error("For condition must be a boolean expression.");
        }
    }
    if (node.increment) {
        visit(*node.increment);
    }
    visit(*node.body);
    symbolTable.leaveScope();
    return nullptr;
}


std::shared_ptr<Type> SemanticAnalyzer::visit(TypeNameAST& node) {
    return nullptr;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(BorrowExprAST& node) {
    auto exprType = visit(*node.expression);
    if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.expression.get())) {
        Symbol* symbol = symbolTable.findSymbol(varExpr->name);
        if (node.isMutable) {
            if (!symbol->isMutable) {
                throw std::runtime_error("Cannot mutably borrow immutable variable '" + varExpr->name + "'.");
            }
            if (symbol->mutableBorrow || symbol->immutableBorrows > 0) {
                throw std::runtime_error("Cannot mutably borrow '" + varExpr->name + "' as it is already borrowed.");
            }
            symbol->mutableBorrow = true;
            symbol->borrowedInScope = true;
        } else {
            if (symbol->mutableBorrow) {
                throw std::runtime_error("Cannot immutably borrow '" + varExpr->name + "' as it is already mutably borrowed.");
            }
            symbol->immutableBorrows++;
            symbol->borrowedInScope = true;
        }

        }
    }
    node.type = std::make_shared<ReferenceType>(exprType, node.isMutable);
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ReferenceTypeAST& node) {
    auto referencedType = typeResolver.resolve(*node.referencedType);
    return std::make_shared<ReferenceType>(referencedType, node.isMutable);
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ArrayLiteralExprAST& node) {
    if (node.elements.empty()) {
        throw std::runtime_error("Array literals cannot be empty.");
    }

    auto firstElementType = visit(*node.elements[0]);
    for (size_t i = 1; i < node.elements.size(); ++i) {
        auto elementType = visit(*node.elements[i]);
        if (elementType->toString() != firstElementType->toString()) {
            throw std::runtime_error("All elements in an array literal must have the same type.");
        }
    }

    // This is a bit of a hack. We'll create a static array type for now, and the
    // variable declaration will convert it to a dynamic array type if needed.
    node.type = std::make_shared<ArrayType>(firstElementType, node.elements.size());
    return node.type;
}

std::shared_ptr<Type> SemanticAnalyzer::visit(ArrayIndexExprAST& node) {
    auto arrayType = visit(*node.array);
    if (!arrayType->isArray() && !arrayType->isDynamicArray()) {
        throw std::runtime_error("Cannot index into a non-array type.");
    }

    auto indexType = visit(*node.index);
    if (!indexType->isInteger()) {
        throw std::runtime_error("Array index must be an integer.");
    }

    if (arrayType->isArray()) {
        node.type = static_cast<ArrayType*>(arrayType.get())->elementType;
    } else {
        node.type = static_cast<DynamicArrayType*>(arrayType.get())->elementType;
    }
    return node.type;
}
