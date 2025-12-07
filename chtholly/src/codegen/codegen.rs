use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::{PointerValue, IntValue};
use crate::parser::ast::{Program, Node, Statement, Expression};
use std::collections::HashMap;

pub struct CodeGen<'ctx> {
    context: &'ctx Context,
    builder: Builder<'ctx>,
    module: Module<'ctx>,
    variables: HashMap<String, PointerValue<'ctx>>,
}

impl<'ctx> CodeGen<'ctx> {
    pub fn new(context: &'ctx Context) -> Self {
        let builder = context.create_builder();
        let module = context.create_module("chtholly");
        CodeGen {
            context,
            builder,
            module,
            variables: HashMap::new(),
        }
    }

    pub fn compile(&mut self, program: Program) {
        let i32_type = self.context.i32_type();
        let fn_type = i32_type.fn_type(&[], false);
        let function = self.module.add_function("main", fn_type, None);
        let basic_block = self.context.append_basic_block(function, "entry");
        self.builder.position_at_end(basic_block);

        for node in program.nodes {
            match node {
                Node::Statement(statement) => self.compile_statement(statement),
                _ => {},
            }
        }

        self.builder.build_return(Some(&i32_type.const_int(0, false))).unwrap();

        self.module.print_to_stderr();
    }

    fn compile_statement(&mut self, statement: Statement) {
        match statement {
            Statement::Let { name, initializer, .. } => {
                let i32_type = self.context.i32_type();
                let alloca = self.builder.build_alloca(i32_type, &name).unwrap();
                let value = self.compile_expression(*initializer);
                self.builder.build_store(alloca, value).unwrap();
                self.variables.insert(name, alloca);
            }
        }
    }

    fn compile_expression(&self, expression: Expression) -> IntValue<'ctx> {
        match expression {
            Expression::IntegerLiteral(value) => {
                self.context.i32_type().const_int(value as u64, false)
            }
        }
    }
}
