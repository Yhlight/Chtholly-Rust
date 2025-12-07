// src/main.rs

use chtholly_lib::parser;

fn main() {
    println!("Welcome to the Chtholly compiler!");
    let input = "fn main(): void {}";
    match parser::parse(input) {
        Ok(_) => println!("Parsing successful!"),
        Err(e) => println!("Parsing failed: {}", e),
    }
}
