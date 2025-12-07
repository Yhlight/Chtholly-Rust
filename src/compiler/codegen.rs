use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{BasicValueEnum, FunctionValue, PointerValue};
use inkwell::IntPredicate;
use std::collections::HashMap;

use crate::compiler::lexer::Token;
use crate::compiler::parser::{Expression, Statement};

#[allow(dead_code)]
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
        let i64_type = self.context.i64_type();
        let fn_type = i64_type.fn_type(&[], false);
        let function = self.module.add_function("main", fn_type, None);
        let basic_block = self.context.append_basic_block(function, "entry");
        self.builder.position_at_end(basic_block);

        for statement in program {
            self.compile_statement(statement)?;
        }

        if self.builder.get_insert_block().unwrap().get_terminator().is_none() {
            self.builder.build_return(Some(&i64_type.const_int(0, false))).map_err(|e| e.to_string())?;
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
        let ptr = self.builder.build_alloca(value.get_type(), &name).map_err(|e| e.to_string())?;
        self.builder.build_store(ptr, value).map_err(|e| e.to_string())?;
        self.variables.insert(name, ptr);
        Ok(())
    }

    fn compile_return_statement(&mut self, expr: Expression) -> Result<(), String> {
        let value = self.compile_expression(expr)?;
        self.builder.build_return(Some(&value)).map_err(|e| e.to_string())?;
        Ok(())
    }

    fn compile_expression(&mut self, expr: Expression) -> Result<BasicValueEnum<'ctx>, String> {
        match expr {
            Expression::IntegerLiteral(value) => Ok(self.context.i64_type().const_int(value as u64, false).into()),
            Expression::BooleanLiteral(value) => Ok(self.context.bool_type().const_int(value as u64, false).into()),
            Expression::Prefix(op, right) => self.compile_prefix_expression(op, *right),
            Expression::Infix(left, op, right) => self.compile_infix_expression(*left, op, *right),
            Expression::Identifier(name) => self.compile_identifier_expression(&name),
            _ => Err("Unsupported expression type".to_string()),
        }
    }

    fn compile_identifier_expression(&self, name: &str) -> Result<BasicValueEnum<'ctx>, String> {
        match self.variables.get(name) {
            Some(ptr) => {
                let loaded_value = self.builder.build_load(self.context.i64_type(), *ptr, name).map_err(|e| e.to_string())?;
                Ok(loaded_value)
            }
            None => Err(format!("Undefined variable: {}", name)),
        }
    }

    fn compile_prefix_expression(&mut self, op: Token, right: Expression) -> Result<BasicValueEnum<'ctx>, String> {
        let right_val = self.compile_expression(right)?;
        match op {
            Token::Bang => {
                let right_int = right_val.into_int_value();
                let zero = self.context.bool_type().const_int(0, false);
                let result = self.builder.build_int_compare(IntPredicate::EQ, right_int, zero, "bang").map_err(|e| e.to_string())?;
                Ok(result.into())
            }
            Token::Minus => {
                let right_int = right_val.into_int_value();
                let result = self.builder.build_int_neg(right_int, "neg").map_err(|e| e.to_string())?;
                Ok(result.into())
            }
            _ => Err(format!("Unsupported prefix operator: {:?}", op)),
        }
    }

    fn compile_infix_expression(&mut self, left: Expression, op: Token, right: Expression) -> Result<BasicValueEnum<'ctx>, String> {
        let left_val = self.compile_expression(left)?;
        let right_val = self.compile_expression(right)?;

        let left_int = left_val.into_int_value();
        let right_int = right_val.into_int_value();

        let result = match op {
            Token::Plus => self.builder.build_int_add(left_int, right_int, "add"),
            Token::Minus => self.builder.build_int_sub(left_int, right_int, "sub"),
            Token::Asterisk => self.builder.build_int_mul(left_int, right_int, "mul"),
            Token::Slash => self.builder.build_int_signed_div(left_int, right_int, "div"),
            Token::Percent => self.builder.build_int_signed_rem(left_int, right_int, "rem"),
            Token::EqualEqual => self.builder.build_int_compare(IntPredicate::EQ, left_int, right_int, "eq"),
            Token::NotEqual => self.builder.build_int_compare(IntPredicate::NE, left_int, right_int, "ne"),
            Token::LessThan => self.builder.build_int_compare(IntPredicate::SLT, left_int, right_int, "lt"),
            Token::LessThanOrEqual => self.builder.build_int_compare(IntPredicate::SLE, left_int, right_int, "le"),
            Token::GreaterThan => self.builder.build_int_compare(IntPredicate::SGT, left_int, right_int, "gt"),
            Token::GreaterThanOrEqual => self.builder.build_int_compare(IntPredicate::SGE, left_int, right_int, "ge"),
            _ => return Err(format!("Unsupported infix operator: {:?}", op)),
        }
        .map_err(|e| e.to_string())?;

        Ok(result.into())
    }

    pub fn get_module(&self) -> &Module<'ctx> {
        &self.module
    }
}
