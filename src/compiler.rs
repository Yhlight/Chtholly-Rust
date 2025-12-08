use inkwell::builder::{Builder, BuilderError};
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{FunctionValue, PointerValue, BasicValueEnum};
use std::collections::HashMap;
use thiserror::Error;
use crate::ast::{ASTNode, Type};

#[derive(Debug, Error)]
pub enum CompileError {
    #[error("Builder error: {0}")]
    BuilderError(#[from] BuilderError),
    #[error("Missing type annotation")]
    MissingTypeAnnotation,
}

type CompileResult<T> = Result<T, CompileError>;

pub enum Value<'ctx> {
    Integer(inkwell::values::IntValue<'ctx>),
    Float(inkwell::values::FloatValue<'ctx>),
    Bool(inkwell::values::IntValue<'ctx>),
    String(inkwell::values::PointerValue<'ctx>),
}

impl<'ctx> Value<'ctx> {
    fn to_basic_value(self) -> BasicValueEnum<'ctx> {
        match self {
            Value::Integer(v) => v.into(),
            Value::Float(v) => v.into(),
            Value::Bool(v) => v.into(),
            Value::String(v) => v.into(),
        }
    }

    fn get_type(&self) -> Type {
        match self {
            Value::Integer(_) => Type::I32,
            Value::Float(_) => Type::F64,
            Value::Bool(_) => Type::Bool,
            Value::String(_) => Type::String,
        }
    }
}

pub struct Compiler<'ctx> {
    context: &'ctx Context,
    builder: Builder<'ctx>,
    module: Module<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
}

impl<'ctx> Compiler<'ctx> {
    pub fn new(context: &'ctx Context) -> Self {
        let builder = context.create_builder();
        let module = context.create_module("chtholly");
        Compiler {
            context,
            builder,
            module,
            variables: HashMap::new(),
        }
    }

    pub fn compile(&mut self, ast: &[ASTNode]) -> CompileResult<()> {
        for node in ast {
            self.compile_node(node)?;
        }
        Ok(())
    }

    fn compile_node(&mut self, node: &ASTNode) -> CompileResult<()> {
        match node {
            ASTNode::Function { name, body, .. } => {
                let _function = self.compile_function(name, body)?;
            }
            _ => {}
        }
        Ok(())
    }

    fn compile_function(&mut self, name: &str, body: &[ASTNode]) -> CompileResult<FunctionValue<'ctx>> {
        let i32_type = self.context.i32_type();
        let fn_type = i32_type.fn_type(&[], false);
        let function = self.module.add_function(name, fn_type, None);
        let basic_block = self.context.append_basic_block(function, "entry");

        self.builder.position_at_end(basic_block);

        for node in body {
            self.compile_statement(node)?;
        }

        self.builder.build_return(Some(&i32_type.const_int(0, false)))?;

        Ok(function)
    }

    fn compile_statement(&mut self, node: &ASTNode) -> CompileResult<()> {
        match node {
            ASTNode::VariableDeclaration {
                name,
                type_annotation,
                value,
                ..
            } => {
                let initial_value = value
                    .as_ref()
                    .map(|v| self.compile_expression(v))
                    .transpose()?;

                let ty = match (type_annotation, &initial_value) {
                    (Some(ty), _) => ty.clone(),
                    (None, Some(val)) => val.get_type(),
                    (None, None) => return Err(CompileError::MissingTypeAnnotation),
                };

                let alloca = match ty {
                    Type::I32 => self.builder.build_alloca(self.context.i32_type(), name)?,
                    Type::F64 => self.builder.build_alloca(self.context.f64_type(), name)?,
                    Type::Bool => self.builder.build_alloca(self.context.bool_type(), name)?,
                    Type::String => self.builder.build_alloca(self.context.ptr_type(inkwell::AddressSpace::default()), name)?,
                };

                if let Some(initial_value) = initial_value {
                    self.builder.build_store(alloca, initial_value.to_basic_value())?;
                }

                self.variables.insert(name.clone(), alloca);
            }
            _ => {}
        }
        Ok(())
    }

    fn compile_expression(&self, node: &ASTNode) -> CompileResult<Value<'ctx>> {
        match node {
            ASTNode::IntegerLiteral(value) => Ok(Value::Integer(self.context.i32_type().const_int(*value as u64, false))),
            ASTNode::FloatLiteral(value) => Ok(Value::Float(self.context.f64_type().const_float(*value))),
            ASTNode::BoolLiteral(value) => Ok(Value::Bool(self.context.bool_type().const_int(*value as u64, false))),
            ASTNode::StringLiteral(value) => {
                let str_ptr = self.builder.build_global_string_ptr(value, ".str")?;
                Ok(Value::String(str_ptr.as_pointer_value()))
            }
            _ => todo!(),
        }
    }

    pub fn print_ir(&self) {
        self.module.print_to_stderr();
    }
}
