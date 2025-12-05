mod lexer;
mod ast;
mod parser;

use lexer::Lexer;
use parser::Parser;

fn main() {
    let input = r#"
        let x = 5;
        let y = 10.0;
        let z = "hello";
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    println!("{:#?}", program);
}
