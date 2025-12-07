//! Generates LLVM IR from the Abstract Syntax Tree (AST).

use crate::ast;
use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::types::{BasicMetadataTypeEnum, BasicTypeEnum, BasicType};
use inkwell::values::{BasicValueEnum, FunctionValue, PointerValue, ValueKind};
use inkwell::IntPredicate;
use std::collections::HashMap;

struct Symbol<'ctx> {
    pointer: PointerValue<'ctx>,
    symbol_type: ast::Type,
    // TODO: Enforce mutability
    is_mutable: bool,
}

struct SymbolTable<'ctx> {
    scopes: Vec<HashMap<String, Symbol<'ctx>>>,
}

impl<'ctx> SymbolTable<'ctx> {
    fn new() -> Self {
        SymbolTable {
            scopes: vec![HashMap::new()],
        }
    }

    fn enter_scope(&mut self) {
        self.scopes.push(HashMap::new());
    }

    fn leave_scope(&mut self) {
        self.scopes.pop();
    }

    fn add_symbol(&mut self, name: String, symbol: Symbol<'ctx>) {
        self.scopes.last_mut().unwrap().insert(name, symbol);
    }

    fn get_symbol(&self, name: &str) -> Option<&Symbol<'ctx>> {
        for scope in self.scopes.iter().rev() {
            if let Some(symbol) = scope.get(name) {
                return Some(symbol);
            }
        }
        None
    }
}

pub struct CodeGen<'ctx> {
    context: &'ctx Context,
    pub module: Module<'ctx>,
    builder: Builder<'ctx>,
    symbol_table: SymbolTable<'ctx>,
    fn_value_opt: Option<FunctionValue<'ctx>>,
}

impl<'ctx> CodeGen<'ctx> {
    pub fn new(context: &'ctx Context, module_name: &str) -> Self {
        let module = context.create_module(module_name);
        let builder = context.create_builder();
        CodeGen {
            context,
            module,
            builder,
            symbol_table: SymbolTable::new(),
            fn_value_opt: None,
        }
    }

    pub fn generate_ir(&mut self, ast: &ast::Program) -> Result<String, String> {
        for function in &ast.functions {
            self.codegen_function(function)?;
        }
        Ok(self.module.print_to_string().to_string())
    }

    fn codegen_function(&mut self, function: &ast::FunctionDefinition) -> Result<(), String> {
        let fn_value = self.codegen_prototype(function)?;
        self.fn_value_opt = Some(fn_value);

        let entry = self.context.append_basic_block(fn_value, "entry");
        self.builder.position_at_end(entry);

        self.symbol_table.enter_scope();

        for statement in &function.body {
            self.codegen_statement(statement)?;
        }

        self.symbol_table.leave_scope();

        // Add a default return if the function doesn't have one
        if self.builder.get_insert_block().unwrap().get_terminator().is_none() {
             if let Some(function) = self.fn_value_opt {
                if function.get_type().get_return_type().is_some() {
                    let _ = self.builder.build_return(Some(&self.context.i32_type().const_int(0, false)));
                } else {
                    let _ = self.builder.build_return(None);
                }
            }
        }


        Ok(())
    }

    fn codegen_prototype(&self, function: &ast::FunctionDefinition) -> Result<FunctionValue<'ctx>, String> {
        let param_types: Vec<BasicMetadataTypeEnum> = function
            .params
            .iter()
            .map(|p| self.to_basic_type(&p.param_type).into())
            .collect();

        let fn_type = match &function.return_type {
             ast::Type::Generic(name, _) if name == "Result" => {
                self.context.i32_type().fn_type(&param_types, false)
             }
             ast::Type::Simple(name)  if name == "i32" => self.context.i32_type().fn_type(&param_types, false),
            _ => self.context.void_type().fn_type(&param_types, false),
        };

        let fn_value = self.module.add_function(&function.name, fn_type, None);

        for (i, param) in fn_value.get_param_iter().enumerate() {
            param.set_name(&function.params[i].name);
        }

        Ok(fn_value)
    }

    fn codegen_statement(&mut self, statement: &ast::Statement) -> Result<(), String> {
        match statement {
            ast::Statement::Expression(expr) => {
                self.codegen_expression(expr)?;
                Ok(())
            }
            ast::Statement::VariableDeclaration(var_decl) => {
                self.codegen_variable_declaration(var_decl)
            }
        }
    }

    fn codegen_variable_declaration(&mut self, var_decl: &ast::VariableDeclaration) -> Result<(), String> {
        let var_type = var_decl.var_type.as_ref().ok_or_else(|| "Variable type must be specified for now".to_string())?;
        let llvm_type = self.to_basic_type(var_type);
        let alloca = self.builder.build_alloca(llvm_type, &var_decl.name).unwrap();

        if let Some(initializer) = &var_decl.initializer {
            let initial_value = self.codegen_expression(initializer)?;
            let casted_value = if initial_value.is_int_value() && initial_value.into_int_value().get_type().get_bit_width() == 1 {
                self.builder.build_int_z_extend(initial_value.into_int_value(), llvm_type.into_int_type(), "zexttmp").unwrap()
            } else {
                self.builder.build_int_cast(initial_value.into_int_value(), llvm_type.into_int_type(), "casttmp").unwrap()
            };
            self.builder.build_store(alloca, casted_value).unwrap();
        }

        let symbol = Symbol {
            pointer: alloca,
            symbol_type: var_type.clone(),
            is_mutable: var_decl.is_mutable,
        };
        self.symbol_table.add_symbol(var_decl.name.clone(), symbol);

        Ok(())
    }

    fn codegen_expression(&mut self, expr: &ast::Expression) -> Result<BasicValueEnum<'ctx>, String> {
        match expr {
            ast::Expression::IntegerLiteral(val) => {
                Ok(self.context.i64_type().const_int(*val as u64, false).into())
            }
            ast::Expression::BooleanLiteral(val) => {
                Ok(self.context.bool_type().const_int(*val as u64, false).into())
            }
            ast::Expression::StringLiteral(val) => {
                let ptr = self.builder.build_global_string_ptr(val, ".str").unwrap();
                Ok(ptr.as_pointer_value().into())
            }
            ast::Expression::Variable(name) => {
                let symbol = self.symbol_table.get_symbol(name).ok_or_else(|| format!("Variable '{}' not found", name))?;
                let llvm_type = self.to_basic_type(&symbol.symbol_type);
                Ok(self.builder.build_load(llvm_type,symbol.pointer, name).unwrap())
            }
            ast::Expression::BinaryExpression { op, left, right } => {
                let left = self.codegen_expression(left)?.into_int_value();
                let right = self.codegen_expression(right)?.into_int_value();

                let result = match op {
                    ast::BinaryOperator::Add => self.builder.build_int_add(left, right, "addtmp").unwrap().into(),
                    ast::BinaryOperator::Subtract => self.builder.build_int_sub(left, right, "subtmp").unwrap().into(),
                    ast::BinaryOperator::Multiply => self.builder.build_int_mul(left, right, "multmp").unwrap().into(),
                    ast::BinaryOperator::Divide => self.builder.build_int_signed_div(left, right, "divtmp").unwrap().into(),
                    ast::BinaryOperator::Modulo => self.builder.build_int_signed_rem(left, right, "modtmp").unwrap().into(),
                    ast::BinaryOperator::Equal => self.builder.build_int_compare(IntPredicate::EQ, left, right, "eqtmp").unwrap().into(),
                    ast::BinaryOperator::NotEqual => self.builder.build_int_compare(IntPredicate::NE, left, right, "netmp").unwrap().into(),
                    ast::BinaryOperator::LessThan => self.builder.build_int_compare(IntPredicate::SLT, left, right, "lttmp").unwrap().into(),
                    ast::BinaryOperator::GreaterThan => self.builder.build_int_compare(IntPredicate::SGT, left, right, "gttmp").unwrap().into(),
                    ast::BinaryOperator::LessThanOrEqual => self.builder.build_int_compare(IntPredicate::SLE, left, right, "letmp").unwrap().into(),
                    ast::BinaryOperator::GreaterThanOrEqual => self.builder.build_int_compare(IntPredicate::SGE, left, right, "getmp").unwrap().into(),
                    ast::BinaryOperator::LogicalAnd => self.builder.build_and(left, right, "andtmp").unwrap().into(),
                    ast::BinaryOperator::LogicalOr => self.builder.build_or(left, right, "ortmp").unwrap().into(),
                };
                Ok(result)
            }
            ast::Expression::UnaryExpression { op, operand } => {
                let operand = self.codegen_expression(operand)?.into_int_value();
                let result = match op {
                    ast::UnaryOperator::Not => self.builder.build_int_compare(IntPredicate::EQ, operand, self.context.bool_type().const_int(0, false), "nottmp").unwrap(),
                };
                Ok(result.into())
            }
            ast::Expression::FunctionCall { name, args } => {
                 let callee = self.module.get_function(name).ok_or(format!("Function '{}' not found in module", name))
                 .or_else(|_err| {
                      if name == "puts" {
                        let i8_ptr_type = self.context.ptr_type(inkwell::AddressSpace::default());
                        let puts_type = self.context.i32_type().fn_type(&[i8_ptr_type.into()], false);
                        Ok(self.module.add_function("puts", puts_type, None))
                    } else {
                        Err(format!("Function '{}' not found", name))
                    }
                 })?;

                let mut compiled_args = Vec::new();
                for arg in args {
                    compiled_args.push(self.codegen_expression(arg)?.into());
                }

                let call = self.builder.build_call(callee, &compiled_args, "calltmp").unwrap();

                 match call.try_as_basic_value() {
                    ValueKind::Basic(value) => Ok(value),
                    _ => Err("Function call did not return a value".to_string())
                }
            }
        }
    }

    fn to_basic_type(&self, ty: &ast::Type) -> BasicTypeEnum<'ctx> {
        match ty {
            ast::Type::Simple(name) => match name.as_str() {
                "i32" => self.context.i32_type().into(),
                "string" => self.context.ptr_type(inkwell::AddressSpace::default()).into(),
                "bool" => self.context.bool_type().into(),
                _ => unimplemented!("Type '{}' not yet supported", name),
            },
            ast::Type::Array(t) => {
                 let inner_type = self.to_basic_type(t);
                 let addr_space = inkwell::AddressSpace::default();
                 inner_type.ptr_type(addr_space).into()
            },
            ast::Type::Generic(name, _) => {
                if name == "Result" {
                     self.context.i32_type().into() //HACK for now
                } else {
                    unimplemented!("Generic types not yet supported")
                }
            },
        }
    }
}
