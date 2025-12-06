
use crate::ast::{Program, Statement, Expression};
use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::FunctionValue;

pub struct Compiler<'a, 'ctx> {
    context: &'ctx Context,
    builder: &'a Builder<'ctx>,
    module: &'a Module<'ctx>,
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
        }
    }

    pub fn compile(&self, program: Program) -> Result<FunctionValue<'ctx>, &'static str> {
        println!("Compiling program: {:?}", program.statements);
        for statement in program.statements {
            self.compile_statement(&statement)?;
        }
        // For now, we'll just return the last function we compiled.
        // This is a bit of a hack, but it will work for our single main function test.
        self.module.get_last_function().ok_or("No function found")
    }

    fn compile_statement(&self, stmt: &Statement) -> Result<(), &'static str> {
        println!("Compiling statement: {:?}", stmt);
        match stmt {
            Statement::ExpressionStatement { expression } => {
                self.compile_expression(expression)?;
            }
            _ => return Err("Unsupported statement"),
        }
        Ok(())
    }

    fn compile_expression(&self, expr: &Expression) -> Result<(), &'static str> {
        println!("Compiling expression: {:?}", expr);
        match expr {
            Expression::FunctionLiteral {
                body,
                ..
            } => {
                let i32_type = self.context.i32_type();
                let fn_type = i32_type.fn_type(&[], false);
                let function = self.module.add_function("main", fn_type, None);
                let basic_block = self.context.append_basic_block(function, "entry");
                self.builder.position_at_end(basic_block);
                self.compile_block_statement(body)?;
                // Every function needs to return a value. For now, we'll just return 0.
                let _ = self.builder.build_return(Some(&i32_type.const_int(0, false)));
            }
            _ => return Err("Unsupported expression"),
        }
        Ok(())
    }

    fn compile_block_statement(&self, stmts: &Statement) -> Result<(), &'static str> {
        if let Statement::Block(stmts) = stmts {
            for stmt in stmts {
                self.compile_statement(stmt)?;
            }
        }
        Ok(())
    }
}
