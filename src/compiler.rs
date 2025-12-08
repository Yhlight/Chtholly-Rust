use inkwell::builder::{Builder, BuilderError};
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{FunctionValue, PointerValue};
use std::collections::HashMap;
use thiserror::Error;
use crate::ast::ASTNode;

#[derive(Debug, Error)]
pub enum CompileError {
    #[error("Builder error: {0}")]
    BuilderError(#[from] BuilderError),
}

type CompileResult<T> = Result<T, CompileError>;

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
                value,
                ..
            } => {
                let i32_type = self.context.i32_type();
                let alloca = self.builder.build_alloca(i32_type, name)?;

                if let Some(value) = value {
                    if let ASTNode::IntegerLiteral(value) = **value {
                        let const_value = i32_type.const_int(value as u64, false);
                        self.builder.build_store(alloca, const_value)?;
                    }
                }

                self.variables.insert(name.clone(), alloca);
            }
            _ => {}
        }
        Ok(())
    }

    pub fn print_ir(&self) {
        self.module.print_to_stderr();
    }
}
