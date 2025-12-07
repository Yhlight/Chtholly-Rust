use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::FunctionValue;
use crate::parser::{Program, Statement};

pub struct CodeGen<'ctx> {
    context: &'ctx Context,
    builder: Builder<'ctx>,
    module: Module<'ctx>,
}

impl<'ctx> CodeGen<'ctx> {
    pub fn new(context: &'ctx Context) -> Self {
        let builder = context.create_builder();
        let module = context.create_module("main");
        CodeGen {
            context,
            builder,
            module,
        }
    }

    pub fn compile(&self, program: Program) -> Result<String, String> {
        let i32_type = self.context.i32_type();
        let fn_type = i32_type.fn_type(&[], false);
        let function = self.module.add_function("main", fn_type, None);
        let basic_block = self.context.append_basic_block(function, "entry");
        self.builder.position_at_end(basic_block);

        for statement in program {
            self.compile_statement(statement, function)?;
        }

        let _ = self.builder.build_return(Some(&i32_type.const_int(0, false)));

        Ok(self.module.print_to_string().to_string())
    }

    fn compile_statement(&self, statement: Statement, function: FunctionValue) -> Result<(), String> {
        match statement {
            Statement::Let(name, _expr) => {
                // For now, we'll just declare a variable on the stack
                let i32_type = self.context.i32_type();
                let alloca = self.builder.build_alloca(i32_type, &name);
                // We'll deal with the expression later
            }
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;
    use crate::parser::Parser;

    #[test]
    fn test_empty_main() {
        let context = Context::create();
        let codegen = CodeGen::new(&context);
        let program = vec![];
        let result = codegen.compile(program).unwrap();
        assert!(result.contains("define i32 @main()"));
        assert!(result.contains("ret i32 0"));
    }

    #[test]
    fn test_let_statement_codegen() {
        let context = Context::create();
        let codegen = CodeGen::new(&context);
        let input = "let x = 5;".to_string();
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let result = codegen.compile(program).unwrap();
        assert!(result.contains("%x = alloca i32"));
    }
}
