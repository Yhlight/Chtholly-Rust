mod parser;
mod codegen;
mod type_checker;

use codegen::codegen::CodeGen;
use inkwell::context::Context;
use parser::lexer::Lexer;
use parser::parser::Parser;
use type_checker::TypeChecker;

fn main() {
    let input = "let x = 5;";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let context = Context::create();
    let mut codegen = CodeGen::new(&context);
    let type_checker = TypeChecker::new();

    type_checker.check_program(&program);
    codegen.compile(program);
}
