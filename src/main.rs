use chtholly::parser::Parser;

fn main() {
    let code = r#"
        // This is a comment.
        fn main() {
            // Another comment.
        }
    "#;

    let parser = Parser::new(code);
    let ast = parser.parse();

    println!("{:#?}", ast);
}
