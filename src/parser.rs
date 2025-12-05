//! Parser for the Chtholly language.

use crate::ast;

/// Parses the Chtholly source code.
pub fn parse(source: &str) -> Result<ast::MainFunction, String> {
    // For now, we'll just return a dummy MainFunction.
    // We'll implement the actual parsing logic later.
    Ok(ast::MainFunction {})
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_main_function() {
        let source = r#"
            fn main(args: string[]): Result<int, SystemError>
            {

            }
        "#;
        assert!(parse(source).is_ok());
    }
}
