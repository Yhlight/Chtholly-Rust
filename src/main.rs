use inkwell::context::Context;

pub mod ast;
pub mod codegen;
pub mod lexer;
pub mod parser;
pub mod semantics;

fn main() {
    let input = "let x = 10; let y = x + 5;";

    let lexer = lexer::Lexer::new(input);
    let mut parser = parser::Parser::new(lexer);
    let ast = parser.parse_program();

    let context = Context::create();
    let mut codegen = codegen::CodeGenerator::new(&context, "main");

    match codegen.compile(&ast) {
        Ok(ir) => println!("{}", ir),
        Err(e) => eprintln!("Error during code generation: {}", e),
    }
}
