//! Code generator for the Chtholly language.

use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::FloatValue;

use crate::ast::{Expression, BinaryOperator};

pub struct CodeGenerator<'ctx> {
    context: &'ctx Context,
    module: Module<'ctx>,
    builder: Builder<'ctx>,
}

impl<'ctx> CodeGenerator<'ctx> {
    pub fn new(context: &'ctx Context, module_name: &str) -> Self {
        let module = context.create_module(module_name);
        let builder = context.create_builder();
        CodeGenerator {
            context,
            module,
            builder,
        }
    }

    fn compile_expression(&self, expr: &Expression) -> Result<FloatValue<'ctx>, &'static str> {
        match expr {
            Expression::NumberLiteral(n) => {
                let f64_type = self.context.f64_type();
                Ok(f64_type.const_float(*n))
            }
            Expression::BinaryExpression { left, operator, right } => {
                let lhs = self.compile_expression(left)?;
                let rhs = self.compile_expression(right)?;

                match operator {
                    BinaryOperator::Plus => Ok(self.builder.build_float_add(lhs, rhs, "addtmp").unwrap()),
                    BinaryOperator::Minus => Ok(self.builder.build_float_sub(lhs, rhs, "subtmp").unwrap()),
                    BinaryOperator::Multiply => Ok(self.builder.build_float_mul(lhs, rhs, "multmp").unwrap()),
                    BinaryOperator::Divide => Ok(self.builder.build_float_div(lhs, rhs, "divtmp").unwrap()),
                }
            }
            _ => Err("Unsupported expression type"),
        }
    }

    pub fn compile(&self, ast: &Expression) -> Result<String, &'static str> {
        let f64_type = self.context.f64_type();
        let fn_type = f64_type.fn_type(&[], false);
        let function = self.module.add_function("main", fn_type, None);
        let basic_block = self.context.append_basic_block(function, "entry");

        self.builder.position_at_end(basic_block);

        let result = self.compile_expression(ast)?;
        self.builder.build_return(Some(&result)).unwrap();

        Ok(self.module.print_to_string().to_string())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;
    use crate::parser::Parser;

    #[test]
    fn test_codegen_number_literal() {
        let context = Context::create();
        let codegen = CodeGenerator::new(&context, "test");
        let expr = Expression::NumberLiteral(42.0);
        let ir = codegen.compile(&expr).unwrap();
        assert!(ir.contains("define double @main()"));
        assert!(ir.contains("ret double 4.200000e+01"));
    }

    #[test]
    fn test_codegen_binary_expression() {
        let context = Context::create();
        let codegen = CodeGenerator::new(&context, "test");
        let input = "1 + 2 * 3"; // This will be constant folded to 7.0
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let expr = parser.parse_expression().unwrap();
        let ir = codegen.compile(&expr).unwrap();

        assert!(ir.contains("define double @main()"));
        assert!(ir.contains("ret double 7.000000e+00"));
    }

    #[test]
    fn test_codegen_complex_binary_expression() {
        let context = Context::create();
        let codegen = CodeGenerator::new(&context, "test");
        let input = "10 / 2 - 3 * 2 + 1"; // (10 / 2) - (3 * 2) + 1 = 5 - 6 + 1 = 0
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let expr = parser.parse_expression().unwrap();
        let ir = codegen.compile(&expr).unwrap();

        assert!(ir.contains("define double @main()"));
        assert!(ir.contains("ret double 0.000000e+00"));
    }
}
