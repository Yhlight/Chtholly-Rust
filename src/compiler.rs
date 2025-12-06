
use crate::ast::{Expression, Program, Statement};
use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{FunctionValue, IntValue, PointerValue};
use std::collections::HashMap;

pub struct Compiler<'a, 'ctx> {
    context: &'ctx Context,
    builder: &'a Builder<'ctx>,
    module: &'a Module<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
}

impl<'a, 'ctx> Compiler<'a, 'ctx> {
    pub fn new(
        context: &'ctx Context,
        builder: &'a Builder<'ctx>,
        module: &'a Module<'ctx>,
    ) -> Self {
        Self {
            context,
            builder,
            module,
            variables: HashMap::new(),
        }
    }

    pub fn compile(&mut self, program: Program) -> Result<FunctionValue<'ctx>, &'static str> {
        let mut last_fn = None;
        if program.statements.len() == 1 {
            if let Statement::ExpressionStatement { expression } = &program.statements[0] {
                 if !matches!(expression, Expression::FunctionLiteral { .. }) {
                    let i32_type = self.context.i32_type();
                    let fn_type = i32_type.fn_type(&[], false);
                    let function = self.module.add_function("main", fn_type, None);
                    let basic_block = self.context.append_basic_block(function, "entry");
                    self.builder.position_at_end(basic_block);
                    let value = self.compile_expression_in_function(expression)?;
                    self.builder.build_return(Some(&value)).unwrap();
                    return Ok(function);
                }
            }
        }

        for statement in program.statements {
            self.compile_statement(&statement, &mut last_fn)?;
        }
        last_fn.ok_or("No function found in program")
    }

    fn compile_statement(
        &mut self,
        stmt: &Statement,
        last_fn: &mut Option<FunctionValue<'ctx>>,
    ) -> Result<(), &'static str> {
        match stmt {
            Statement::ExpressionStatement { expression } => {
                if let Expression::FunctionLiteral { .. } = expression {
                    let function = self.compile_function_literal(expression)?;
                    *last_fn = Some(function);
                    Ok(())
                } else {
                    Err("Unsupported top-level expression.")
                }
            }
            _ => Err("Unsupported top-level statement."),
        }
    }

    fn compile_function_literal(
        &mut self,
        expr: &Expression,
    ) -> Result<FunctionValue<'ctx>, &'static str> {
        if let Expression::FunctionLiteral { body, .. } = expr {
            let i32_type = self.context.i32_type();
            let fn_type = i32_type.fn_type(&[], false);
            let function = self.module.add_function("main", fn_type, None);
            let basic_block = self.context.append_basic_block(function, "entry");

            self.builder.position_at_end(basic_block);
            self.variables.clear();

            self.compile_block_statement(body)?;

            if basic_block.get_terminator().is_none() {
                self.builder
                    .build_return(Some(&i32_type.const_int(0, false)))
                    .unwrap();
            }

            Ok(function)
        } else {
            Err("Not a function literal")
        }
    }

    fn compile_block_statement(&mut self, stmts: &Statement) -> Result<(), &'static str> {
        if let Statement::Block(stmts) = stmts {
            for stmt in stmts {
                self.compile_statement_in_function(stmt)?;
            }
        }
        Ok(())
    }

    fn compile_statement_in_function(&mut self, stmt: &Statement) -> Result<(), &'static str> {
        match stmt {
            Statement::Let { name, value, .. } => {
                let i32_type = self.context.i32_type();
                let alloca = self.builder.build_alloca(i32_type, &name.value).unwrap();
                let val = self.compile_expression_in_function(value)?;
                self.builder.build_store(alloca, val).unwrap();
                self.variables.insert(name.value.clone(), alloca);
            }
            Statement::Mut { name, value, .. } => {
                let i32_type = self.context.i32_type();
                let alloca = self.builder.build_alloca(i32_type, &name.value).unwrap();
                let val = self.compile_expression_in_function(value)?;
                self.builder.build_store(alloca, val).unwrap();
                self.variables.insert(name.value.clone(), alloca);
            }
            _ => return Err("Unsupported statement inside function"),
        }
        Ok(())
    }

    fn compile_expression_in_function(
        &self,
        expr: &Expression,
    ) -> Result<IntValue<'ctx>, &'static str> {
        match expr {
            Expression::IntegerLiteral { value, .. } => {
                let i32_type = self.context.i32_type();
                Ok(i32_type.const_int(*value as u64, false))
            }
            Expression::InfixExpression {
                left,
                operator,
                right,
                ..
            } => {
                let left = self.compile_expression_in_function(left)?;
                let right = self.compile_expression_in_function(right)?;
                match operator.as_str() {
                    "+" => Ok(self.builder.build_int_add(left, right, "addtmp").unwrap()),
                    "-" => Ok(self.builder.build_int_sub(left, right, "subtmp").unwrap()),
                    "*" => Ok(self.builder.build_int_mul(left, right, "multmp").unwrap()),
                    "/" => Ok(self.builder.build_int_signed_div(left, right, "divtmp").unwrap()),
                    _ => Err("Unknown operator"),
                }
            }
            _ => Err("Unsupported expression inside function"),
        }
    }
}
