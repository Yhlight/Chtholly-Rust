use app::parser;

fn main() {
    let source = r#"
        // This is a comment.
        fn main() {}
    "#;

    match parser::parse(source) {
        Ok(ast) => {
            println!("Parsed successfully: {:#?}", ast);
        }
        Err(e) => {
            eprintln!("Parse failed: {}", e);
        }
    }
}
