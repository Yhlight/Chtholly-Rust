use inkwell::context::Context;

pub mod ast;
pub mod codegen;
pub mod lexer;
pub mod parser;

fn main() {
    let input = "1 + 2 * 3";

    let lexer = lexer::Lexer::new(input);
    let mut parser = parser::Parser::new(lexer);
    let ast = parser.parse_expression().expect("Failed to parse expression");

    let context = Context::create();
    let codegen = codegen::CodeGenerator::new(&context, "main");

    match codegen.compile(&ast) {
        Ok(ir) => println!("{}", ir),
        Err(e) => eprintln!("Error during code generation: {}", e),
    }
}
