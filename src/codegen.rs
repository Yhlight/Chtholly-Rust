use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::PointerValue;
use std::collections::HashMap;

use crate::ast::{Expression, Identifier, InfixOperator, Literal, Program, Statement};

#[derive(Clone)]
pub struct Variable<'ctx> {
    pub ptr: PointerValue<'ctx>,
    pub is_mutable: bool,
}

pub struct Compiler<'ctx> {
    pub context: &'ctx Context,
    pub builder: Builder<'ctx>,
    pub module: Module<'ctx>,

    variables: HashMap<String, Variable<'ctx>>,
}

impl<'ctx> Compiler<'ctx> {
    pub fn new(context: &'ctx Context) -> Self {
        let builder = context.create_builder();
        let module = context.create_module("main");

        Compiler {
            context,
            builder,
            module,
            variables: HashMap::new(),
        }
    }

    pub fn compile_program(&mut self, program: Program) {
        let main_fn = self
            .module
            .add_function("main", self.context.i32_type().fn_type(&[], false), None);
        let entry = self.context.append_basic_block(main_fn, "entry");
        self.builder.position_at_end(entry);

        for statement in program.statements {
            self.compile_statement(statement);
        }

        self.builder
            .build_return(Some(&self.context.i32_type().const_int(0, false)))
            .unwrap();
    }

    fn compile_statement(&mut self, statement: Statement) {
        match statement {
            Statement::Let(identifier, is_mutable, expression) => {
                self.compile_let_statement(identifier, is_mutable, expression);
            }
            Statement::Expression(expression) => {
                self.compile_expression(expression);
            }
            _ => unimplemented!(),
        }
    }

    fn compile_let_statement(
        &mut self,
        identifier: Identifier,
        is_mutable: bool,
        expression: Expression,
    ) {
        let name = identifier.0;
        let value = self.compile_expression(expression);

        let i32_type = self.context.i32_type();
        let alloca = self.builder.build_alloca(i32_type, &name).unwrap();
        self.builder
            .build_store(alloca, value.into_int_value())
            .unwrap();
        self.variables.insert(
            name,
            Variable {
                ptr: alloca,
                is_mutable,
            },
        );
    }

    fn compile_expression(
        &mut self,
        expression: Expression,
    ) -> inkwell::values::BasicValueEnum<'ctx> {
        match expression {
            Expression::Literal(literal) => self.compile_literal(literal),
            Expression::Identifier(identifier) => {
                let name = identifier.0;
                let variable = self.variables.get(&name).unwrap();
                self.builder
                    .build_load(self.context.i32_type(), variable.ptr, &name)
                    .unwrap()
            }
            Expression::Infix(left, op, right) => {
                if op == InfixOperator::Assign {
                    let name = match *left {
                        Expression::Identifier(id) => id.0,
                        _ => panic!("Expected identifier on the left side of assignment"),
                    };

                    let variable = self.variables.get(&name).unwrap().clone();

                    if !variable.is_mutable {
                        panic!("Cannot assign to immutable variable");
                    }

                    let value = self.compile_expression(*right);

                    self.builder
                        .build_store(variable.ptr, value.into_int_value())
                        .unwrap();
                    return value;
                }

                let left_val = self.compile_expression(*left).into_int_value();
                let right_val = self.compile_expression(*right).into_int_value();

                let result = match op {
                    InfixOperator::Plus => self.builder.build_int_add(left_val, right_val, "addtmp").unwrap(),
                    InfixOperator::Minus => self.builder.build_int_sub(left_val, right_val, "subtmp").unwrap(),
                    InfixOperator::Multiply => self.builder.build_int_mul(left_val, right_val, "multmp").unwrap(),
                    InfixOperator::Divide => self.builder.build_int_signed_div(left_val, right_val, "divtmp").unwrap(),
                    _ => unimplemented!(),
                };

                result.into()
            }
            _ => unimplemented!(),
        }
    }

    fn compile_literal(&mut self, literal: Literal) -> inkwell::values::BasicValueEnum<'ctx> {
        match literal {
            Literal::Integer(value) => self.context.i32_type().const_int(value as u64, false).into(),
            _ => unimplemented!(),
        }
    }
}
