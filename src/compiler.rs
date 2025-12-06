
use crate::ast::{Expression, Program, Statement};
use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{FunctionValue, IntValue, PointerValue};
use inkwell::IntPredicate;
use std::collections::HashMap;

pub struct Compiler<'a, 'ctx> {
    context: &'ctx Context,
    builder: &'a Builder<'ctx>,
    module: &'a Module<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
    function: Option<FunctionValue<'ctx>>,
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
                    let i32_type = self.context.i32_type();
                    let fn_type = i32_type.fn_type(&[], false);
                    let function = self.module.add_function("main", fn_type, None);
                    self.function = Some(function);
                    let basic_block = self.context.append_basic_block(function, "entry");
                    self.builder.position_at_end(basic_block);
                    let value = self.compile_expression_in_function(expression)?;
                    self.builder.build_return(Some(&value)).map_err(|_| "Failed to build return")?;
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
                let i32_type = self.context.i32_type();
                let alloca = self.builder.build_alloca(i32_type, &name.value).map_err(|_| "Failed to build alloca")?;
                let val = self.compile_expression_in_function(value)?;
                self.builder.build_store(alloca, val).map_err(|_| "Failed to build store")?;
                self.variables.insert(name.value.clone(), alloca);
            }
            Statement::Mut { name, value, .. } => {
                let i32_type = self.context.i32_type();
                let alloca = self.builder.build_alloca(i32_type, &name.value).map_err(|_| "Failed to build alloca")?;
                let val = self.compile_expression_in_function(value)?;
                self.builder.build_store(alloca, val).map_err(|_| "Failed to build store")?;
                self.variables.insert(name.value.clone(), alloca);
            }
            Statement::ExpressionStatement { expression } => {
                self.compile_expression_in_function(expression)?;
            }
            _ => return Err("Unsupported statement inside function"),
        }
        Ok(())
    }

    fn compile_expression_in_function(
        &mut self,
        expr: &Expression,
    ) -> Result<IntValue<'ctx>, &'static str> {
        match expr {
             Expression::Identifier(ident) => {
                let ptr = self.variables.get(&ident.value).ok_or("Unknown variable")?;
                let i32_type = self.context.i32_type();
                Ok(self.builder.build_load(i32_type, *ptr, &ident.value).map_err(|_| "Failed to build load")?.into_int_value())
            }
            Expression::IntegerLiteral { value, .. } => {
                let i32_type = self.context.i32_type();
                Ok(i32_type.const_int(*value as u64, false))
            }
            Expression::BooleanLiteral { value, .. } => {
                let i1_type = self.context.bool_type();
                Ok(i1_type.const_int(*value as u64, false))
            }
            Expression::PrefixExpression { operator, right, .. } => {
                let right = self.compile_expression_in_function(right)?;
                match operator.as_str() {
                    "!" => {
                        let i1_type = self.context.bool_type();
                        let zero = i1_type.const_int(0, false);
                        Ok(self.builder.build_int_compare(IntPredicate::EQ, right, zero, "nottmp").map_err(|_| "Failed to build int compare")?)
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
                let left = self.compile_expression_in_function(left)?;
                let right = self.compile_expression_in_function(right)?;
                match operator.as_str() {
                    "+" => Ok(self.builder.build_int_add(left, right, "addtmp").map_err(|_| "Failed to build int add")?),
                    "-" => Ok(self.builder.build_int_sub(left, right, "subtmp").map_err(|_| "Failed to build int sub")?),
                    "*" => Ok(self.builder.build_int_mul(left, right, "multmp").map_err(|_| "Failed to build int mul")?),
                    "/" => Ok(self.builder.build_int_signed_div(left, right, "divtmp").map_err(|_| "Failed to build int signed div")?),
                    "<" => Ok(self.builder.build_int_compare(IntPredicate::SLT, left, right, "cmptmp").map_err(|_| "Failed to build int compare")?),
                    ">" => Ok(self.builder.build_int_compare(IntPredicate::SGT, left, right, "cmptmp").map_err(|_| "Failed to build int compare")?),
                    "==" => Ok(self.builder.build_int_compare(IntPredicate::EQ, left, right, "cmptmp").map_err(|_| "Failed to build int compare")?),
                    "!=" => Ok(self.builder.build_int_compare(IntPredicate::NE, left, right, "cmptmp").map_err(|_| "Failed to build int compare")?),
                    _ => Err("Unknown operator"),
                }
            }
            Expression::IfExpression {
                condition,
                consequence,
                alternative,
                ..
            } => self.compile_if_expression(condition, consequence, alternative),
            _ => Err("Unsupported expression inside function"),
        }
    }

    fn compile_if_expression(
        &mut self,
        condition: &Expression,
        consequence: &Statement,
        alternative: &Option<Box<Statement>>,
    ) -> Result<IntValue<'ctx>, &'static str> {
        let cond = self.compile_expression_in_function(condition)?;

        let then_bb = self.context.append_basic_block(self.function.unwrap(), "then");
        let else_bb = self.context.append_basic_block(self.function.unwrap(), "else");
        let merge_bb = self.context.append_basic_block(self.function.unwrap(), "ifcont");

        self.builder.build_conditional_branch(cond, then_bb, else_bb).map_err(|_| "Failed to build conditional branch")?;

        // Build then block
        self.builder.position_at_end(then_bb);
        let then_val = self.compile_block_for_if(consequence)?;
        self.builder.build_unconditional_branch(merge_bb).map_err(|_| "Failed to build unconditional branch")?;
        let then_bb = self.builder.get_insert_block().unwrap();

        // Build else block
        self.builder.position_at_end(else_bb);
        let else_val = if let Some(alt) = alternative {
            self.compile_block_for_if(alt)?
        } else {
           return Err("if expression without else is not supported yet");
        };
        self.builder.build_unconditional_branch(merge_bb).map_err(|_| "Failed to build unconditional branch")?;
        let else_bb = self.builder.get_insert_block().unwrap();

        // Build merge block
        self.builder.position_at_end(merge_bb);
        let phi = self.builder.build_phi(self.context.i32_type(), "iftmp").map_err(|_| "Failed to build phi")?;
        phi.add_incoming(&[(&then_val, then_bb), (&else_val, else_bb)]);
        Ok(phi.as_basic_value().into_int_value())
    }

    fn compile_block_for_if(
        &mut self,
        stmts: &Statement,
    ) -> Result<IntValue<'ctx>, &'static str> {
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
