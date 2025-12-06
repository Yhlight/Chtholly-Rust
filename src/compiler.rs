use crate::ast::{Expression, Program, Statement};
use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{FunctionValue, IntValue, PointerValue};
use inkwell::IntPredicate;
use std::collections::HashMap;

use inkwell::basic_block::BasicBlock;
use inkwell::values::FloatValue;

#[derive(Debug, Clone)]
pub enum CompilerValue<'ctx> {
    Int(IntValue<'ctx>),
    Float(FloatValue<'ctx>),
    Bool(IntValue<'ctx>),
    String(PointerValue<'ctx>),
    Char(IntValue<'ctx>),
}

impl<'ctx> CompilerValue<'ctx> {
    fn into_int_value(self) -> Result<IntValue<'ctx>, &'static str> {
        match self {
            CompilerValue::Int(iv) => Ok(iv),
            CompilerValue::Bool(iv) => Ok(iv),
            CompilerValue::Char(iv) => Ok(iv),
            CompilerValue::Float(_) => Err("Expected an integer value, got a float"),
            _ => Err("Expected an integer value"),
        }
    }
}

pub struct Compiler<'a, 'ctx> {
    context: &'ctx Context,
    builder: &'a Builder<'ctx>,
    module: &'a Module<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
    function: Option<FunctionValue<'ctx>>,
    loop_cond_bbs: Vec<BasicBlock<'ctx>>,
    loop_end_bbs: Vec<BasicBlock<'ctx>>,
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
            function: None,
            loop_cond_bbs: Vec::new(),
            loop_end_bbs: Vec::new(),
        }
    }

    pub fn compile(&mut self, program: Program) -> Result<FunctionValue<'ctx>, &'static str> {
        let mut main_fn = None;
        let mut functions = Vec::new();

        for statement in program.statements {
            if let Statement::ExpressionStatement { expression } = &statement {
                if let Expression::FunctionLiteral { name, .. } = expression {
                    let function = self.compile_function_literal(expression)?;
                    if name.value == "main" {
                        main_fn = Some(function);
                    }
                    functions.push(function);
                } else {
                    let function = match expression {
                        Expression::FloatLiteral { .. } => {
                            let f64_type = self.context.f64_type();
                            let fn_type = f64_type.fn_type(&[], false);
                            self.module.add_function("main", fn_type, None)
                        }
                        _ => {
                            let i32_type = self.context.i32_type();
                            let fn_type = i32_type.fn_type(&[], false);
                            self.module.add_function("main", fn_type, None)
                        }
                    };

                    self.function = Some(function);
                    let basic_block = self.context.append_basic_block(function, "entry");
                    self.builder.position_at_end(basic_block);

                    let value = self.compile_expression_in_function(expression)?;

                    match value {
                        CompilerValue::Int(iv) | CompilerValue::Bool(iv) | CompilerValue::Char(iv) => {
                            if function.get_type().get_return_type().unwrap().is_int_type() {
                                self.builder.build_return(Some(&iv)).map_err(|_| "Failed to build return")?;
                            } else {
                                return Err("Type mismatch: expected int, function returns float");
                            }
                        }
                        CompilerValue::Float(fv) => {
                            if function.get_type().get_return_type().unwrap().is_float_type() {
                                self.builder.build_return(Some(&fv)).map_err(|_| "Failed to build return")?;
                            } else {
                                return Err("Type mismatch: expected float, function returns int");
                            }
                        }
                        _ => return Err("Unsupported return type for implicit main function"),
                    }
                    return Ok(function);
                }
            } else {
                return Err("Unsupported top-level statement.");
            }
        }

        if let Some(main_fn) = main_fn {
            Ok(main_fn)
        } else if let Some(first_fn) = functions.into_iter().next() {
            Ok(first_fn)
        } else {
            Err("No functions found in program")
        }
    }

    fn compile_function_literal(
        &mut self,
        expr: &Expression,
    ) -> Result<FunctionValue<'ctx>, &'static str> {
        if let Expression::FunctionLiteral { name, body, .. } = expr {
            let i32_type = self.context.i32_type();
            let fn_type = i32_type.fn_type(&[], false);
            let function = self.module.add_function(&name.value, fn_type, None);
            self.function = Some(function);
            let basic_block = self.context.append_basic_block(function, "entry");

            self.builder.position_at_end(basic_block);
            self.variables.clear();

            self.compile_block_statement(body)?;

            if basic_block.get_terminator().is_none() {
                self.builder
                    .build_return(Some(&i32_type.const_int(0, false)))
                    .map_err(|_| "Failed to build return")?;
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
                let val = self
                    .compile_expression_in_function(value)?
                    .into_int_value()?;
                let i32_type = self.context.i32_type();
                let alloca = self
                    .builder
                    .build_alloca(i32_type, &name.value)
                    .map_err(|_| "Failed to build alloca")?;
                self.builder
                    .build_store(alloca, val)
                    .map_err(|_| "Failed to build store")?;
                self.variables.insert(name.value.clone(), alloca);
            }
            Statement::Mut { name, value, .. } => {
                let val = self
                    .compile_expression_in_function(value)?
                    .into_int_value()?;
                let i32_type = self.context.i32_type();
                let alloca = self
                    .builder
                    .build_alloca(i32_type, &name.value)
                    .map_err(|_| "Failed to build alloca")?;
                self.builder
                    .build_store(alloca, val)
                    .map_err(|_| "Failed to build store")?;
                self.variables.insert(name.value.clone(), alloca);
            }
            Statement::ExpressionStatement { expression } => {
                self.compile_expression_in_function(expression)?;
            }
            Statement::Continue(_) => {
                if let Some(cond_bb) = self.loop_cond_bbs.last() {
                    self.builder
                        .build_unconditional_branch(*cond_bb)
                        .map_err(|_| "Failed to build unconditional branch")?;
                } else {
                    return Err("continue used outside of a loop");
                }
            }
            Statement::Break(_) => {
                if let Some(end_bb) = self.loop_end_bbs.last() {
                    self.builder
                        .build_unconditional_branch(*end_bb)
                        .map_err(|_| "Failed to build unconditional branch")?;
                } else {
                    return Err("break used outside of a loop");
                }
            }
            _ => return Err("Unsupported statement inside function"),
        }
        Ok(())
    }

    fn compile_expression_in_function(
        &mut self,
        expr: &Expression,
    ) -> Result<CompilerValue<'ctx>, &'static str> {
        match expr {
            Expression::Identifier(ident) => {
                let ptr = self.variables.get(&ident.value).ok_or("Unknown variable")?;
                let i32_type = self.context.i32_type();
                Ok(CompilerValue::Int(
                    self.builder
                        .build_load(i32_type, *ptr, &ident.value)
                        .map_err(|_| "Failed to build load")?
                        .into_int_value(),
                ))
            }
            Expression::IntegerLiteral { value, .. } => {
                let i32_type = self.context.i32_type();
                Ok(CompilerValue::Int(i32_type.const_int(*value as u64, false)))
            }
            Expression::FloatLiteral { value, .. } => {
                let f64_type = self.context.f64_type();
                Ok(CompilerValue::Float(f64_type.const_float(*value)))
            }
            Expression::BooleanLiteral { value, .. } => {
                let i1_type = self.context.bool_type();
                Ok(CompilerValue::Bool(i1_type.const_int(*value as u64, false)))
            }
            Expression::StringLiteral { value, .. } => {
                let ptr = self
                    .builder
                    .build_global_string_ptr(value, ".str")
                    .map_err(|_| "Failed to build global string")?;
                Ok(CompilerValue::String(ptr.as_pointer_value()))
            }
            Expression::CharLiteral { value, .. } => {
                let i8_type = self.context.i8_type();
                Ok(CompilerValue::Char(
                    i8_type.const_int(*value as u64, false),
                ))
            }
            Expression::PrefixExpression {
                operator, right, ..
            } => {
                let right = self
                    .compile_expression_in_function(right)?
                    .into_int_value()?;
                match operator.as_str() {
                    "!" => {
                        let i1_type = self.context.bool_type();
                        let zero = i1_type.const_int(0, false);
                        Ok(CompilerValue::Bool(
                            self.builder
                                .build_int_compare(IntPredicate::EQ, right, zero, "nottmp")
                                .map_err(|_| "Failed to build int compare")?,
                        ))
                    }
                    _ => Err("Unknown operator"),
                }
            }
            Expression::InfixExpression {
                left,
                operator,
                right,
                ..
            } => {
                let left = self
                    .compile_expression_in_function(left)?
                    .into_int_value()?;
                let right = self
                    .compile_expression_in_function(right)?
                    .into_int_value()?;
                match operator.as_str() {
                    "+" => Ok(CompilerValue::Int(
                        self.builder
                            .build_int_add(left, right, "addtmp")
                            .map_err(|_| "Failed to build int add")?,
                    )),
                    "-" => Ok(CompilerValue::Int(
                        self.builder
                            .build_int_sub(left, right, "subtmp")
                            .map_err(|_| "Failed to build int sub")?,
                    )),
                    "*" => Ok(CompilerValue::Int(
                        self.builder
                            .build_int_mul(left, right, "multmp")
                            .map_err(|_| "Failed to build int mul")?,
                    )),
                    "/" => Ok(CompilerValue::Int(
                        self.builder
                            .build_int_signed_div(left, right, "divtmp")
                            .map_err(|_| "Failed to build int signed div")?,
                    )),
                    "%" => Ok(CompilerValue::Int(
                        self.builder
                            .build_int_signed_rem(left, right, "remtmp")
                            .map_err(|_| "Failed to build int signed rem")?,
                    )),
                    "<" => Ok(CompilerValue::Bool(
                        self.builder
                            .build_int_compare(IntPredicate::SLT, left, right, "cmptmp")
                            .map_err(|_| "Failed to build int compare")?,
                    )),
                    ">" => Ok(CompilerValue::Bool(
                        self.builder
                            .build_int_compare(IntPredicate::SGT, left, right, "cmptmp")
                            .map_err(|_| "Failed to build int compare")?,
                    )),
                    "==" => Ok(CompilerValue::Bool(
                        self.builder
                            .build_int_compare(IntPredicate::EQ, left, right, "cmptmp")
                            .map_err(|_| "Failed to build int compare")?,
                    )),
                    "!=" => Ok(CompilerValue::Bool(
                        self.builder
                            .build_int_compare(IntPredicate::NE, left, right, "cmptmp")
                            .map_err(|_| "Failed to build int compare")?,
                    )),
                    ">=" => Ok(CompilerValue::Bool(
                        self.builder
                            .build_int_compare(IntPredicate::SGE, left, right, "cmptmp")
                            .map_err(|_| "Failed to build int compare")?,
                    )),
                    "<=" => Ok(CompilerValue::Bool(
                        self.builder
                            .build_int_compare(IntPredicate::SLE, left, right, "cmptmp")
                            .map_err(|_| "Failed to build int compare")?,
                    )),
                    "&&" => Ok(CompilerValue::Bool(
                        self.builder
                            .build_and(left, right, "andtmp")
                            .map_err(|_| "Failed to build and")?,
                    )),
                    "||" => Ok(CompilerValue::Bool(
                        self.builder
                            .build_or(left, right, "ortmp")
                            .map_err(|_| "Failed to build or")?,
                    )),
                    _ => Err("Unknown operator"),
                }
            }
            Expression::IfExpression {
                condition,
                consequence,
                alternative,
                ..
            } => self.compile_if_expression(condition, consequence, alternative),
            Expression::WhileExpression {
                condition, body, ..
            } => self.compile_while_expression(condition, body),
            _ => Err("Unsupported expression inside function"),
        }
    }

    fn compile_while_expression(
        &mut self,
        condition: &Expression,
        body: &Statement,
    ) -> Result<CompilerValue<'ctx>, &'static str> {
        let function = self.function.unwrap();
        let cond_bb = self.context.append_basic_block(function, "loop_cond");
        let body_bb = self.context.append_basic_block(function, "loop_body");
        let end_bb = self.context.append_basic_block(function, "loop_end");

        self.loop_cond_bbs.push(cond_bb);
        self.loop_end_bbs.push(end_bb);

        self.builder
            .build_unconditional_branch(cond_bb)
            .map_err(|_| "Failed to build unconditional branch")?;
        self.builder.position_at_end(cond_bb);

        let cond = self
            .compile_expression_in_function(condition)?
            .into_int_value()?;
        self.builder
            .build_conditional_branch(cond, body_bb, end_bb)
            .map_err(|_| "Failed to build conditional branch")?;

        self.builder.position_at_end(body_bb);
        self.compile_block_statement(body)?;

        if self.builder.get_insert_block().unwrap().get_terminator().is_none() {
            self.builder
                .build_unconditional_branch(cond_bb)
                .map_err(|_| "Failed to build unconditional branch")?;
        }

        self.builder.position_at_end(end_bb);

        self.loop_cond_bbs.pop();
        self.loop_end_bbs.pop();

        Ok(CompilerValue::Int(
            self.context.i32_type().const_int(0, false),
        ))
    }

    fn compile_if_expression(
        &mut self,
        condition: &Expression,
        consequence: &Statement,
        alternative: &Option<Box<Statement>>,
    ) -> Result<CompilerValue<'ctx>, &'static str> {
        let cond = self
            .compile_expression_in_function(condition)?
            .into_int_value()?;

        let then_bb = self
            .context
            .append_basic_block(self.function.unwrap(), "then");
        let else_bb = self
            .context
            .append_basic_block(self.function.unwrap(), "else");
        let merge_bb = self
            .context
            .append_basic_block(self.function.unwrap(), "ifcont");

        self.builder
            .build_conditional_branch(cond, then_bb, else_bb)
            .map_err(|_| "Failed to build conditional branch")?;

        // Build then block
        self.builder.position_at_end(then_bb);
        let then_val = self.compile_block_for_if(consequence)?;
        self.builder
            .build_unconditional_branch(merge_bb)
            .map_err(|_| "Failed to build unconditional branch")?;
        let then_bb = self.builder.get_insert_block().unwrap();

        // Build else block
        self.builder.position_at_end(else_bb);
        let else_val = if let Some(alt) = alternative {
            self.compile_block_for_if(alt)?
        } else {
            return Err("if expression without else is not supported yet");
        };
        self.builder
            .build_unconditional_branch(merge_bb)
            .map_err(|_| "Failed to build unconditional branch")?;
        let else_bb = self.builder.get_insert_block().unwrap();

        // Build merge block
        self.builder.position_at_end(merge_bb);
        let phi = self
            .builder
            .build_phi(self.context.i32_type(), "iftmp")
            .map_err(|_| "Failed to build phi")?;
        phi.add_incoming(&[
            (&then_val.into_int_value()?, then_bb),
            (&else_val.into_int_value()?, else_bb),
        ]);
        Ok(CompilerValue::Int(phi.as_basic_value().into_int_value()))
    }

    fn compile_block_for_if(
        &mut self,
        stmts: &Statement,
    ) -> Result<CompilerValue<'ctx>, &'static str> {
        if let Statement::Block(stmts) = stmts {
            let mut last_val = None;
            for stmt in stmts {
                if let Statement::ExpressionStatement { expression } = stmt {
                    last_val = Some(self.compile_expression_in_function(expression)?);
                } else {
                    self.compile_statement_in_function(stmt)?;
                }
            }
            last_val.ok_or("if block must end with an expression")
        } else {
            Err("Not a block statement")
        }
    }
}
