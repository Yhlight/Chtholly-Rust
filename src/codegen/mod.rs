//! Code generator for the Chtholly language.

use inkwell::builder::Builder;
use inkwell::context::Context;
use inkwell::module::Module;
use inkwell::values::FloatValue;

use crate::ast::{Program, Statement, Expression, BinaryOperator};
use crate::semantics::SymbolTable;

pub struct CodeGenerator<'ctx> {
    context: &'ctx Context,
    module: Module<'ctx>,
    builder: Builder<'ctx>,
    symbol_table: SymbolTable<'ctx>,
}

impl<'ctx> CodeGenerator<'ctx> {
    pub fn new(context: &'ctx Context, module_name: &str) -> Self {
        let module = context.create_module(module_name);
        let builder = context.create_builder();
        let symbol_table = SymbolTable::new();
        CodeGenerator {
            context,
            module,
            builder,
            symbol_table,
        }
    }

    fn compile_expression(&mut self, expr: &Expression) -> Result<FloatValue<'ctx>, &'static str> {
        match expr {
            Expression::NumberLiteral(n) => {
                let f64_type = self.context.f64_type();
                Ok(f64_type.const_float(*n))
            }
            Expression::Identifier(name) => {
                let var = self.symbol_table.get(name).ok_or("Undefined variable")?;
                let f64_type = self.context.f64_type();
                let loaded_val = self.builder.build_load(f64_type,var.pointer, name).unwrap();
                Ok(loaded_val.into_float_value())
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

    fn compile_statement(&mut self, stmt: &Statement) -> Result<(), &'static str> {
        match stmt {
            Statement::LetDeclaration { name, is_mutable, initializer } => {
                let f64_type = self.context.f64_type();
                let alloca = self.builder.build_alloca(f64_type, name).unwrap();

                if let Some(expr) = initializer {
                    let value = self.compile_expression(expr)?;
                    self.builder.build_store(alloca, value).unwrap();
                }

                self.symbol_table.define(name, *is_mutable, alloca);
                Ok(())
            }
            Statement::ExpressionStatement(expr) => {
                self.compile_expression(expr)?;
                Ok(())
            }
        }
    }

    pub fn compile(&mut self, ast: &Program) -> Result<String, &'static str> {
        let f64_type = self.context.f64_type();
        let fn_type = f64_type.fn_type(&[], false);
        let function = self.module.add_function("main", fn_type, None);
        let basic_block = self.context.append_basic_block(function, "entry");

        self.builder.position_at_end(basic_block);

        for stmt in &ast.statements {
            self.compile_statement(stmt)?;
        }

        // For now, we'll just return 0.0 from main.
        self.builder.build_return(Some(&f64_type.const_float(0.0))).unwrap();

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
        let mut codegen = CodeGenerator::new(&context, "test");
        let input = "42;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let ir = codegen.compile(&program).unwrap();

        // Although the expression is compiled, it's not used.
        // The function should still be valid.
        assert!(ir.contains("define double @main()"));
    }

    #[test]
    fn test_codegen_binary_expression() {
        let context = Context::create();
        let mut codegen = CodeGenerator::new(&context, "test");
        let input = "1 + 2 * 3;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let ir = codegen.compile(&program).unwrap();

        // Similar to the number literal test, the expression is compiled but unused.
        assert!(ir.contains("define double @main()"));
    }

    #[test]
    fn test_codegen_let_statement() {
        let context = Context::create();
        let mut codegen = CodeGenerator::new(&context, "test");
        let input = "let x = 42;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let ir = codegen.compile(&program).unwrap();

        assert!(ir.contains("%x = alloca double"));
        assert!(ir.contains("store double 4.200000e+01, ptr %x"));
    }

    #[test]
    fn test_codegen_identifier_expression() {
        let context = Context::create();
        let mut codegen = CodeGenerator::new(&context, "test");
        let input = "let x = 42; x;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        let ir = codegen.compile(&program).unwrap();

        assert!(ir.contains("%x = alloca double"));
        assert!(ir.contains("store double 4.200000e+01, ptr %x"));
        assert!(ir.contains("load double, ptr %x"));
    }
}
