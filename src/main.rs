use std::fs;
use crate::lexer::Lexer;
use crate::parser::Parser;

pub mod lexer;
pub mod ast;
pub mod parser;

fn main() {
    let input = fs::read_to_string("input.cns").expect("Something went wrong reading the file");
    let lexer = Lexer::new(&input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    match program {
        Ok(program) => println!("{:#?}", program),
        Err(errors) => {
            for error in errors {
                println!("{}", error);
            }
        }
    }
}
