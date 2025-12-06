mod token;
mod lexer;
mod ast;
mod parser;

use lexer::Lexer;
use parser::Parser;

fn main() {
    let input = "let x = 5;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    if !parser.errors().is_empty() {
        for error in parser.errors() {
            println!("{}", error);
        }
    }

    for statement in program.statements {
        println!("{:?}", statement);
    }
}
