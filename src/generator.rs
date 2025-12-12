use std::collections::HashMap;
use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{PointerValue, FunctionValue};
use crate::ast::{Program, Statement, Expression};

pub struct CodeGenerator<'ctx> {
    context: &'ctx Context,
    module: Module<'ctx>,
    builder: Builder<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
    current_function: Option<FunctionValue<'ctx>>,
}

impl<'ctx> CodeGenerator<'ctx> {
    pub fn new(context: &'ctx Context) -> Self {
        let module = context.create_module("main");
        let builder = context.create_builder();
        CodeGenerator {
            context,
            module,
            builder,
            variables: HashMap::new(),
            current_function: None,
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
            Statement::Let(name, expr) => {
                let alloca = self.create_entry_block_alloca(name);
                let value = self.generate_expression(expr)?;
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
        }
        Ok(())
    }

    fn generate_expression(&mut self, expression: &Expression) -> Result<inkwell::values::BasicValueEnum<'ctx>, &'static str> {
        match expression {
            Expression::Identifier(name) => {
                let ptr = self.variables.get(name).ok_or("undeclared variable")?;
                let value = self.builder.build_load(self.context.i64_type(), *ptr, name.as_str()).map_err(|_| "Failed to build load")?;
                Ok(value)
            }
            Expression::Integer(value) => {
                Ok(self.context.i64_type().const_int(*value as u64, false).into())
            }
            Expression::Infix(left, op, right) => {
                let left_val = self.generate_expression(left)?;
                let right_val = self.generate_expression(right)?;

                match op {
                    crate::lexer::Token::Plus => {
                        let result = self.builder.build_int_add(left_val.into_int_value(), right_val.into_int_value(), "addtmp").map_err(|_| "Failed to build int add")?;
                        Ok(result.into())
                    }
                    _ => Err("unsupported operator"),
                }
            }
            _ => Err("unsupported expression"),
        }
    }

    fn create_entry_block_alloca(&self, name: &str) -> PointerValue<'ctx> {
        let builder = self.context.create_builder();
        let entry = self.current_function.unwrap().get_first_basic_block().unwrap();

        match entry.get_first_instruction() {
            Some(first_instr) => builder.position_before(&first_instr),
            None => builder.position_at_end(entry),
        }

        builder.build_alloca(self.context.i64_type(), name).unwrap()
    }

    pub fn print_to_string(&self) -> String {
        self.module.print_to_string().to_string()
    }
}
