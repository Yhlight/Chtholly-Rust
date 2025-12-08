use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::PointerValue;
use inkwell::IntPredicate;
use std::collections::HashMap;

use crate::ast::{BlockStatement, Expression, Identifier, InfixOperator, Literal, Program, Statement};

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

    fn compile_block_statement(&mut self, block: BlockStatement) -> Option<inkwell::values::BasicValueEnum<'ctx>> {
        let mut last_val = None;
        for stmt in block.statements {
            match stmt {
                Statement::Expression(expr) => {
                    last_val = Some(self.compile_expression(expr));
                }
                _ => self.compile_statement(stmt),
            }
        }
        last_val
    }

    fn compile_let_statement(
        &mut self,
        identifier: Identifier,
        is_mutable: bool,
        expression: Expression,
    ) {
        let name = identifier.0;
        let value = self.compile_expression(expression);

        let ty = value.get_type();
        let alloca = self.builder.build_alloca(ty, &name).unwrap();
        self.builder.build_store(alloca, value).unwrap();
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
                // This is still a simplification. A proper type system is needed.
                let ty: inkwell::types::BasicTypeEnum = if name == "true" || name == "false" {
                    self.context.bool_type().into()
                } else {
                    self.context.i32_type().into()
                };

                self.builder.build_load(ty, variable.ptr, &name).unwrap()
            }
            Expression::Infix(left, op, right) => self.compile_infix_expression(*left, op, *right),
            Expression::If { condition, consequence, alternative } => {
                self.compile_if_expression(*condition, consequence, alternative)
            }
            _ => unimplemented!(),
        }
    }

    fn compile_infix_expression(
        &mut self,
        left: Expression,
        op: InfixOperator,
        right: Expression,
    ) -> inkwell::values::BasicValueEnum<'ctx> {
        if op == InfixOperator::Assign {
            let name = match left {
                Expression::Identifier(id) => id.0,
                _ => panic!("Expected identifier on the left side of assignment"),
            };

            let variable = self.variables.get(&name).unwrap().clone();

            if !variable.is_mutable {
                panic!("Cannot assign to immutable variable");
            }

            let value = self.compile_expression(right);

            self.builder.build_store(variable.ptr, value).unwrap();
            return value;
        }

        let left_val = self.compile_expression(left).into_int_value();
        let right_val = self.compile_expression(right).into_int_value();

        match op {
            InfixOperator::Plus => self.builder.build_int_add(left_val, right_val, "addtmp").unwrap().into(),
            InfixOperator::Minus => self.builder.build_int_sub(left_val, right_val, "subtmp").unwrap().into(),
            InfixOperator::Multiply => self.builder.build_int_mul(left_val, right_val, "multmp").unwrap().into(),
            InfixOperator::Divide => self.builder.build_int_signed_div(left_val, right_val, "divtmp").unwrap().into(),
            InfixOperator::Equal => self.builder.build_int_compare(IntPredicate::EQ, left_val, right_val, "eqtmp").unwrap().into(),
            InfixOperator::NotEqual => self.builder.build_int_compare(IntPredicate::NE, left_val, right_val, "netmp").unwrap().into(),
            InfixOperator::LessThan => self.builder.build_int_compare(IntPredicate::SLT, left_val, right_val, "lttmp").unwrap().into(),
            InfixOperator::GreaterThan => self.builder.build_int_compare(IntPredicate::SGT, left_val, right_val, "gttmp").unwrap().into(),
            _ => unimplemented!(),
        }
    }

    fn compile_if_expression(
        &mut self,
        condition: Expression,
        consequence: BlockStatement,
        alternative: Option<BlockStatement>,
    ) -> inkwell::values::BasicValueEnum<'ctx> {
        let parent_function = self.builder.get_insert_block().unwrap().get_parent().unwrap();

        let cond = self.compile_expression(condition).into_int_value();

        let then_bb = self.context.append_basic_block(parent_function, "then");
        let else_bb = self.context.append_basic_block(parent_function, "else");
        let merge_bb = self.context.append_basic_block(parent_function, "ifcont");

        self.builder.build_conditional_branch(cond, then_bb, else_bb).unwrap();

        // Build then block
        self.builder.position_at_end(then_bb);
        let then_val = self.compile_block_statement(consequence).unwrap();
        self.builder.build_unconditional_branch(merge_bb).unwrap();
        let then_bb = self.builder.get_insert_block().unwrap();

        // Build else block
        self.builder.position_at_end(else_bb);
        let else_val = if let Some(alt_block) = alternative {
            self.compile_block_statement(alt_block).unwrap()
        } else {
            // If there's no else, the expression implicitly returns a default value.
            // For now, let's use 0 for i32. A proper type system would handle this better.
            self.context.i32_type().const_int(0, false).into()
        };
        self.builder.build_unconditional_branch(merge_bb).unwrap();
        let else_bb = self.builder.get_insert_block().unwrap();

        // Build merge block
        self.builder.position_at_end(merge_bb);
        let phi = self.builder.build_phi(self.context.i32_type(), "iftmp").unwrap();
        phi.add_incoming(&[(&then_val, then_bb), (&else_val, else_bb)]);

        phi.as_basic_value()
    }


    fn compile_literal(&mut self, literal: Literal) -> inkwell::values::BasicValueEnum<'ctx> {
        match literal {
            Literal::Integer(value) => self.context.i32_type().const_int(value as u64, false).into(),
            Literal::Boolean(value) => self.context.bool_type().const_int(value as u64, false).into(),
            _ => unimplemented!(),
        }
    }
}
