//! Parser for the Chtholly language.

use crate::ast::{self, Expression, Function, LetDeclaration, Literal, Parameter, Statement, Type};
use chumsky::prelude::*;

fn type_parser() -> impl Parser<char, Type, Error = Simple<char>> {
    let ident = text::ident().padded();

    recursive(|type_parser| {
        let generic_args = type_parser
            .clone()
            .separated_by(just(',').padded())
            .allow_trailing()
            .delimited_by(just('<'), just('>'));

        let base_type = ident
            .then(generic_args.or_not())
            .map(|(name, args)| match args {
                Some(args) => Type::Generic(name, args),
                None => Type::Simple(name),
            });

        base_type
            .then(just("[]").repeated())
            .foldl(|ty, _brackets| Type::Array(Box::new(ty)))
    })
}

fn expression_parser() -> impl Parser<char, Expression, Error = Simple<char>> {
    let integer = text::int(10).map(|s: String| Expression::Literal(Literal::Int(s.parse().unwrap())));
    integer
}

fn statement_parser() -> impl Parser<char, Statement, Error = Simple<char>> {
    let let_declaration = just("let")
        .padded()
        .ignore_then(text::ident().padded())
        .then(just(':').padded().ignore_then(type_parser()).or_not())
        .then_ignore(just('=').padded())
        .then(expression_parser())
        .then_ignore(just(';'))
        .map(|((name, type_info), value)| Statement::Let(LetDeclaration { name, type_info, value }));

    let_declaration
}

fn function_parser() -> impl Parser<char, ast::Node, Error = Simple<char>> {
    let ident = text::ident().padded();

    let param = ident
        .then_ignore(just(':').padded())
        .then(type_parser())
        .map(|(name, type_info)| Parameter { name, type_info });

    let params = param
        .separated_by(just(',').padded())
        .allow_trailing()
        .delimited_by(just('('), just(')'));

    let body = statement_parser()
        .repeated()
        .delimited_by(just('{').padded(), just('}').padded());

    just("fn")
        .padded()
        .ignore_then(ident)
        .then(params)
        .then_ignore(just(':').padded())
        .then(type_parser())
        .then(body)
        .map(|(((name, parameters), return_type), body)| {
            ast::Node::Function(Function {
                name,
                parameters,
                return_type,
                body,
            })
        })
}

/// Parses the Chtholly source code.
pub fn parse(source: &str) -> Result<Vec<ast::Node>, Vec<Simple<char>>> {
    let single_line_comment = just("//").then(take_until(just('\n'))).padded().ignored();
    let multi_line_comment = just("/*").then(take_until(just("*/"))).padded().ignored();
    let comment = single_line_comment.or(multi_line_comment);

    let top_level = function_parser().map(Some).or(comment.to(None));

    let parser = top_level
        .repeated()
        .map(|nodes| nodes.into_iter().flatten().collect())
        .then_ignore(end());

    parser.parse(source)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{Expression, Function, LetDeclaration, Literal, Node, Parameter, Statement, Type};

    #[test]
    fn test_parse_empty_source() {
        assert!(parse("").unwrap().is_empty());
    }

    #[test]
    fn test_parse_comments_only() {
        let source = r#"
            // This is a single-line comment.
            /* This is a multi-line comment. */
        "#;
        assert!(parse(source).unwrap().is_empty());
    }

    #[test]
    fn test_parse_main_function_with_let_declaration() {
        let source = r#"
            fn main(args: string[]): Result<int, SystemError>
            {
                let a = 10;
                let b: int = 20;
            }
        "#;
        let expected_ast = vec![Node::Function(Function {
            name: "main".to_string(),
            parameters: vec![Parameter {
                name: "args".to_string(),
                type_info: Type::Array(Box::new(Type::Simple("string".to_string()))),
            }],
            return_type: Type::Generic(
                "Result".to_string(),
                vec![
                    Type::Simple("int".to_string()),
                    Type::Simple("SystemError".to_string()),
                ],
            ),
            body: vec![
                Statement::Let(LetDeclaration {
                    name: "a".to_string(),
                    type_info: None,
                    value: Expression::Literal(Literal::Int(10)),
                }),
                Statement::Let(LetDeclaration {
                    name: "b".to_string(),
                    type_info: Some(Type::Simple("int".to_string())),
                    value: Expression::Literal(Literal::Int(20)),
                }),
            ],
        })];

        assert_eq!(parse(source).unwrap(), expected_ast);
    }

    #[test]
    fn test_parse_interspersed_comments() {
        let source = r#"
            // Comment before function
            fn first(): void {}

            /* Comment between functions */
            fn second(): void {}
            // Comment after function
        "#;
        let expected_ast = vec![
            Node::Function(Function {
                name: "first".to_string(),
                parameters: vec![],
                return_type: Type::Simple("void".to_string()),
                body: vec![],
            }),
            Node::Function(Function {
                name: "second".to_string(),
                parameters: vec![],
                return_type: Type::Simple("void".to_string()),
                body: vec![],
            }),
        ];

        assert_eq!(parse(source).unwrap(), expected_ast);
    }
}
