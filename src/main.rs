mod ast;
mod parser;

fn main() {
    let source = r#"
        fn main(args: string[]): Result<int, SystemError>
        {

        }
    "#;

    match parser::parse(source) {
        Ok(_) => println!("Parsing successful!"),
        Err(e) => println!("Parsing failed: {}", e),
    }
}
