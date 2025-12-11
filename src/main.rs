mod ast;
mod lexer;
mod parser;

use lexer::lexer::Lexer;
use parser::parser::Parser;

fn main() {
    let input = r#"
        let x = 5;
        let y = 10;
        let foobar = 838383;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);

    let program = parser.parse_program();

    if !parser.errors().is_empty() {
        println!("Parser errors:");
        for error in parser.errors() {
            println!("{}", error);
        }
    }

    println!("Parsed {} statements", program.statements.len());
}
