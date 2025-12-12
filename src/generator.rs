use std::collections::HashMap;
use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{PointerValue, FunctionValue, BasicValueEnum};
use inkwell::types::{BasicTypeEnum, BasicType};
use inkwell::basic_block::BasicBlock;
use crate::ast::{Program, Statement, Expression};
use crate::semantic::{self, SemanticAnalyzer};

pub struct CodeGenerator<'a, 'ctx> {
    context: &'ctx Context,
    module: Module<'ctx>,
    builder: Builder<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
    semantic_analyzer: &'a mut SemanticAnalyzer,
    current_function: Option<FunctionValue<'ctx>>,
    loop_end_blocks: Vec<BasicBlock<'ctx>>,
}

impl<'a, 'ctx> CodeGenerator<'a, 'ctx> {
    pub fn new(context: &'ctx Context, semantic_analyzer: &'a mut SemanticAnalyzer) -> Self {
        let module = context.create_module("main");
        let builder = context.create_builder();
        CodeGenerator {
            context,
            module,
            builder,
            variables: HashMap::new(),
            semantic_analyzer,
            current_function: None,
            loop_end_blocks: Vec::new(),
        }
    }

    pub fn generate(&mut self, program: &Program) -> Result<(), &'static str> {
        let i64_type = self.context.i64_type();
        let main_fn_type = i64_type.fn_type(&[], false);
        let main_fn = self.module.add_function("main", main_fn_type, None);
        let basic_block = self.context.append_basic_block(main_fn, "entry");

        self.builder.position_at_end(basic_block);
        self.current_function = Some(main_fn);

        for statement in program {
            self.generate_statement(statement)?;
        }

        Ok(())
    }

    fn generate_statement(&mut self, statement: &Statement) -> Result<(), &'static str> {
        match statement {
            Statement::Let { name, value, .. } => {
                let ty = self.get_type(value)?;
                let alloca = self.create_entry_block_alloca(name, ty)?;
                let value = self.generate_expression(value)?;
                self.builder.build_store(alloca, value).map_err(|_| "Failed to build store")?;
                self.variables.insert(name.clone(), alloca);
            }
            Statement::Return(expr) => {
                let value = self.generate_expression(expr)?;
                self.builder.build_return(Some(&value)).map_err(|_| "Failed to build return")?;
            }
            Statement::Expression(expr) => {
                self.generate_expression(expr)?;
            }
            Statement::Block(statements) => {
                self.generate_block_statement(statements)?;
            }
            Statement::If { condition, consequence, alternative } => {
                let cond = self.generate_expression(condition)?;

                let then_block = self.context.append_basic_block(self.current_function.unwrap(), "then");
                let else_block = self.context.append_basic_block(self.current_function.unwrap(), "else");
                let merge_block = self.context.append_basic_block(self.current_function.unwrap(), "merge");

                self.builder.build_conditional_branch(cond.into_int_value(), then_block, else_block).map_err(|_| "Failed to build conditional branch")?;

                self.builder.position_at_end(then_block);
                self.generate_statement(consequence)?;
                self.builder.build_unconditional_branch(merge_block).map_err(|_| "Failed to build unconditional branch")?;

                self.builder.position_at_end(else_block);
                if let Some(alt) = alternative {
                    self.generate_statement(alt)?;
                }
                self.builder.build_unconditional_branch(merge_block).map_err(|_| "Failed to build unconditional branch")?;

                self.builder.position_at_end(merge_block);
            }
            Statement::While { condition, body } => {
                let loop_cond_block = self.context.append_basic_block(self.current_function.unwrap(), "loop_cond");
                let loop_body_block = self.context.append_basic_block(self.current_function.unwrap(), "loop_body");
                let loop_end_block = self.context.append_basic_block(self.current_function.unwrap(), "loop_end");

                self.builder.build_unconditional_branch(loop_cond_block).map_err(|_| "Failed to build unconditional branch")?;

                self.builder.position_at_end(loop_cond_block);
                let cond = self.generate_expression(condition)?;
                self.builder.build_conditional_branch(cond.into_int_value(), loop_body_block, loop_end_block).map_err(|_| "Failed to build conditional branch")?;

                self.builder.position_at_end(loop_body_block);
                self.loop_end_blocks.push(loop_end_block);
                self.generate_statement(body)?;
                self.loop_end_blocks.pop();
                self.builder.build_unconditional_branch(loop_cond_block).map_err(|_| "Failed to build unconditional branch")?;

                self.builder.position_at_end(loop_end_block);
            }
            Statement::Break => {
                if let Some(loop_end_block) = self.loop_end_blocks.last() {
                    self.builder.build_unconditional_branch(*loop_end_block).map_err(|_| "Failed to build unconditional branch")?;
                }
            }
        }
        Ok(())
    }

    fn generate_block_statement(&mut self, statements: &[Statement]) -> Result<(), &'static str> {
        self.semantic_analyzer.enter_scope();
        for statement in statements {
            self.generate_statement(statement)?;
        }
        self.semantic_analyzer.leave_scope();
        Ok(())
    }

    fn get_type(&self, expression: &Expression) -> Result<BasicTypeEnum<'ctx>, &'static str> {
        let semantic_type = self.semantic_analyzer.type_of(expression).ok_or("could not determine type")?;
        match semantic_type {
            semantic::Type::Integer => Ok(self.context.i64_type().as_basic_type_enum()),
            semantic::Type::Float => Ok(self.context.f64_type().as_basic_type_enum()),
            semantic::Type::Boolean => Ok(self.context.bool_type().as_basic_type_enum()),
            semantic::Type::Char => Ok(self.context.i8_type().as_basic_type_enum()),
            _ => Err("unsupported type")
        }
    }

    fn generate_expression(&mut self, expression: &Expression) -> Result<BasicValueEnum<'ctx>, &'static str> {
        match expression {
            Expression::Identifier(name) => {
                let ptr = self.variables.get(name).ok_or("undeclared variable")?;
                let ty = self.get_type(expression)?;
                let value = self.builder.build_load(ty, *ptr, name.as_str()).map_err(|_| "Failed to build load")?;
                Ok(value)
            }
            Expression::Integer(value) => {
                Ok(self.context.i64_type().const_int(*value as u64, false).into())
            }
            Expression::Float(value) => {
                Ok(self.context.f64_type().const_float(*value).into())
            }
            Expression::Boolean(value) => {
                Ok(self.context.bool_type().const_int(*value as u64, false).into())
            }
            Expression::Char(value) => {
                Ok(self.context.i8_type().const_int(*value as u64, false).into())
            }
            Expression::Infix(left, op, right) => {
                let left_val = self.generate_expression(left)?;
                let right_val = self.generate_expression(right)?;
                let left_ty = self.get_type(left)?;

                if left_ty.is_float_type() {
                    match op {
                        crate::lexer::Token::Plus => {
                            let result = self.builder.build_float_add(left_val.into_float_value(), right_val.into_float_value(), "addtmp").map_err(|_| "Failed to build float add")?;
                            Ok(result.into())
                        }
                        crate::lexer::Token::Minus => {
                            let result = self.builder.build_float_sub(left_val.into_float_value(), right_val.into_float_value(), "subtmp").map_err(|_| "Failed to build float sub")?;
                            Ok(result.into())
                        }
                        _ => Err("unsupported operator"),
                    }
                } else {
                    match op {
                        crate::lexer::Token::Plus => {
                            let result = self.builder.build_int_add(left_val.into_int_value(), right_val.into_int_value(), "addtmp").map_err(|_| "Failed to build int add")?;
                            Ok(result.into())
                        }
                        crate::lexer::Token::Minus => {
                            let result = self.builder.build_int_sub(left_val.into_int_value(), right_val.into_int_value(), "subtmp").map_err(|_| "Failed to build int sub")?;
                            Ok(result.into())
                        }
                        crate::lexer::Token::Asterisk => {
                            let result = self.builder.build_int_mul(left_val.into_int_value(), right_val.into_int_value(), "multmp").map_err(|_| "Failed to build int mul")?;
                            Ok(result.into())
                        }
                        crate::lexer::Token::Slash => {
                            let result = self.builder.build_int_signed_div(left_val.into_int_value(), right_val.into_int_value(), "divtmp").map_err(|_| "Failed to build int div")?;
                            Ok(result.into())
                        }
                        crate::lexer::Token::EqualEqual => {
                            let result = self.builder.build_int_compare(inkwell::IntPredicate::EQ, left_val.into_int_value(), right_val.into_int_value(), "eqtmp").map_err(|_| "Failed to build int compare")?;
                            Ok(result.into())
                        }
                        _ => Err("unsupported operator"),
                    }
                }
            }
            Expression::Assign { name, value } => {
                let new_value = self.generate_expression(value)?;
                let ptr = self.variables.get(name).ok_or("undeclared variable")?;
                self.builder.build_store(*ptr, new_value).map_err(|_| "Failed to build store")?;
                Ok(new_value)
            }
            _ => Err("unsupported expression"),
        }
    }

    fn create_entry_block_alloca(&self, name: &str, ty: BasicTypeEnum<'ctx>) -> Result<PointerValue<'ctx>, &'static str> {
        let builder = self.context.create_builder();
        let entry = self.current_function.unwrap().get_first_basic_block().unwrap();

        match entry.get_first_instruction() {
            Some(first_instr) => builder.position_before(&first_instr),
            None => builder.position_at_end(entry),
        }

        builder.build_alloca(ty, name).map_err(|_| "Failed to build alloca")
    }

    pub fn print_to_string(&self) -> String {
        self.module.print_to_string().to_string()
    }
}
