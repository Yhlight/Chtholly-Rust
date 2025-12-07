mod compiler;

use std::env;
use std::fs;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: chtholly <file>");
        return;
    }

    let filename = &args[1];
    let source_code = match fs::read_to_string(filename) {
        Ok(code) => code,
        Err(e) => {
            eprintln!("Error reading file: {}", e);
            return;
        }
    };

    println!("Compiling file: {}", filename);
    // TODO: Add calls to the lexer, parser, and code generator.
}
