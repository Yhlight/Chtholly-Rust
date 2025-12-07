use chumsky::{prelude::*, primitive::take_until, text};

#[derive(Debug, PartialEq, Eq, Clone, Hash)]
pub enum Type {
    Simple(String),
    Array(Box<Type>),
    Result(Box<Type>, Box<Type>),
}

#[derive(Debug, PartialEq, Eq, Clone, Hash)]
pub enum AST {
    Function {
        name: String,
        args: Vec<(String, Type)>,
        return_type: Type,
        body: Vec<AST>,
    },
}

pub fn parser<'a>() -> impl Parser<char, Vec<AST>, Error = Simple<char>> {
    let ident = text::ident().padded();

    let line_comment = just("//")
        .ignore_then(take_until(text::newline()))
        .padded()
        .ignored();

    let block_comment = just("/*")
        .ignore_then(take_until(just("*/")))
        .padded()
        .ignored();

    let comments = line_comment.or(block_comment).repeated();

    let r#type = recursive(|r#type| {
        let generic_args = r#type
            .clone()
            .separated_by(just(',').padded())
            .allow_trailing()
            .delimited_by(just('<').padded(), just('>').padded());

        let base_type = text::ident()
            .then(generic_args.or_not())
            .map(|(name, generics)| match generics {
                Some(mut types) if name == "Result" && types.len() == 2 => {
                    Type::Result(Box::new(types.remove(0)), Box::new(types.remove(0)))
                }
                _ => Type::Simple(name),
            });

        base_type
            .then(just("[]").padded().repeated())
            .foldl(|ty, _brackets| Type::Array(Box::new(ty)))
            .padded()
    });

    let function = recursive(|function| {
        just("fn")
            .padded()
            .ignore_then(ident)
            .then(
                ident
                    .then_ignore(just(':').padded())
                    .then(r#type.clone())
                    .separated_by(just(',').padded())
                    .allow_trailing()
                    .delimited_by(just('(').padded(), just(')').padded()),
            )
            .then_ignore(just(':').padded()) // Corrected from "->"
            .then(r#type.clone())
            .then(
                function
                    .repeated()
                    .delimited_by(just('{').padded(), just('}').padded()),
            )
            .map(|(((name, args), return_type), body)| AST::Function {
                name,
                args,
                return_type,
                body,
            })
    });

    comments
        .ignore_then(function.repeated())
        .then_ignore(comments)
        .then_ignore(end())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_main_function() {
        let code = r#"
        // This is a comment
        /*
         * This is a multi-line comment
         */
        fn main(args: string[]): Result<i32, SystemError> {

        }
        "#;
        let result = parser().parse(code);
        assert!(result.is_ok(), "Parser should succeed. Errors: {:?}", result.err());
        let ast = result.unwrap();
        assert_eq!(ast.len(), 1, "Should parse one function");
        match &ast[0] {
            AST::Function { name, args, return_type, body } => {
                assert_eq!(name, "main");
                assert_eq!(args.len(), 1);
                assert_eq!(args[0].0, "args");
                assert_eq!(args[0].1, Type::Array(Box::new(Type::Simple("string".to_string()))));
                assert_eq!(*return_type, Type::Result(Box::new(Type::Simple("i32".to_string())), Box::new(Type::Simple("SystemError".to_string()))));
                assert!(body.is_empty());
            }
        }
    }
}
