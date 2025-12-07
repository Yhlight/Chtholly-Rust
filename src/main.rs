
mod parser;

use pest::Parser;
use crate::parser::{ChthollyParser, Rule};

fn main() {
    let source = r#"
        // this is a line comment
        123 /* this is a block comment */ 456
    "#;
    let pairs = ChthollyParser::parse(Rule::program, source).unwrap_or_else(|e| panic!("{}", e));

    for pair in pairs {
        println!("Rule: {:?}, Text: {}", pair.as_rule(), pair.as_str());
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_comments() {
        let source = r#"
            // line comment
            /* block comment */
        "#;
        let result = ChthollyParser::parse(Rule::program, source);
        assert!(result.is_ok());
    }

    #[test]
    fn test_parse_numbers_and_comments() {
        let source = "123 // comment\n456";
        let result = ChthollyParser::parse(Rule::program, source);
        assert!(result.is_ok());
    }
}
