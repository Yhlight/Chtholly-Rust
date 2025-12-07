use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{FunctionValue, PointerValue};
use std::collections::HashMap;

use crate::compiler::parser::{Expression, Statement};

pub struct CodeGenerator<'ctx> {
    context: &'ctx Context,
    module: Module<'ctx>,
    builder: Builder<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
    function: Option<FunctionValue<'ctx>>,
}

impl<'ctx> CodeGenerator<'ctx> {
    pub fn new(context: &'ctx Context, name: &str) -> Self {
        let module = context.create_module(name);
        let builder = context.create_builder();
        CodeGenerator {
            context,
            module,
            builder,
            variables: HashMap::new(),
            function: None,
        }
    }

    pub fn compile_program(&mut self, program: Vec<Statement>) -> Result<(), String> {
        for statement in program {
            self.compile_statement(statement)?;
        }
        Ok(())
    }

    fn compile_statement(&mut self, statement: Statement) -> Result<(), String> {
        match statement {
            Statement::Let(name, expr) => self.compile_let_statement(name, expr),
            Statement::Return(expr) => self.compile_return_statement(expr),
            Statement::Expression(expr) => self.compile_expression(expr).map(|_| ()),
        }
    }

    fn compile_let_statement(&mut self, name: String, expr: Expression) -> Result<(), String> {
        let value = self.compile_expression(expr)?;
        let ptr = self.builder.build_alloca(self.context.i64_type(), &name).map_err(|e| e.to_string())?;
        self.builder.build_store(ptr, value).map_err(|e| e.to_string())?;
        self.variables.insert(name, ptr);
        Ok(())
    }

    fn compile_return_statement(&mut self, expr: Expression) -> Result<(), String> {
        let value = self.compile_expression(expr)?;
        self.builder.build_return(Some(&value)).map_err(|e| e.to_string())?;
        Ok(())
    }

    fn compile_expression(&mut self, expr: Expression) -> Result<inkwell::values::IntValue<'ctx>, String> {
        match expr {
            Expression::IntegerLiteral(value) => Ok(self.context.i64_type().const_int(value as u64, false)),
            _ => Err("Unsupported expression type".to_string()),
        }
    }

    pub fn get_module(&self) -> &Module<'ctx> {
        &self.module
    }
}
