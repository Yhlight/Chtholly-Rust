//! Generates LLVM IR from the Abstract Syntax Tree (AST).

use crate::ast;
use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::types::{BasicMetadataTypeEnum, BasicType, BasicTypeEnum};
use inkwell::values::{BasicValueEnum, FunctionValue, PointerValue, ValueKind};
use std::collections::HashMap;

pub struct CodeGen<'ctx> {
    context: &'ctx Context,
    pub module: Module<'ctx>,
    builder: Builder<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
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
            variables: HashMap::new(),
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

        for statement in &function.body {
            self.codegen_statement(statement)?;
        }

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
        }
    }

    fn codegen_expression(&mut self, expr: &ast::Expression) -> Result<BasicValueEnum<'ctx>, String> {
        match expr {
            ast::Expression::IntegerLiteral(val) => {
                Ok(self.context.i64_type().const_int(*val as u64, false).into())
            }
            ast::Expression::StringLiteral(val) => {
                let ptr = self.builder.build_global_string_ptr(val, ".str").unwrap();
                Ok(ptr.as_pointer_value().into())
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
