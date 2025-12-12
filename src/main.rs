use std::env;
use std::fs;
use inkwell::context::Context;

mod lexer;
mod ast;
mod parser;
mod semantic;
mod generator;

#[cfg(test)]
mod lexer_test;

#[cfg(test)]
mod parser_test;

#[cfg(test)]
mod semantic_test;

#[cfg(test)]
mod generator_test;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: {} <file>", args[0]);
        return;
    }

    let input = fs::read_to_string(&args[1]).expect("Unable to read file");

    let lexer = lexer::Lexer::new(&input);
    let mut parser = parser::Parser::new(lexer);
    let program = parser.parse_program();

    let mut semantic_analyzer = semantic::SemanticAnalyzer::new();
    semantic_analyzer.analyze(&program);

    if !semantic_analyzer.errors.is_empty() {
        for error in semantic_analyzer.errors {
            eprintln!("Semantic error: {}", error);
        }
        return;
    }

    let context = Context::create();
    let mut generator = generator::CodeGenerator::new(&context);
    match generator.generate(&program) {
        Ok(_) => {
            println!("{}", generator.print_to_string());
        }
        Err(e) => {
            eprintln!("Code generation error: {}", e);
        }
    }
}
