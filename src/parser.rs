//! Parser for the Chtholly language.

use crate::ast::{self, BinaryOp, Expression, Function, LetDeclaration, Literal, Parameter, Statement, Type};
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
    recursive(|expr| {
        let integer = text::int(10).map(|s: String| Literal::Int(s.parse().unwrap()));
        let float = text::int(10)
            .then_ignore(just('.'))
            .then(text::digits(10))
            .map(|(a, b): (String, String)| Literal::Float(format!("{}.{}", a, b).parse().unwrap()));
        let char_literal = none_of('\'')
            .delimited_by(just('\''), just('\''))
            .map(Literal::Char);
        let bool_literal = just("true").to(Literal::Bool(true)).or(just("false").to(Literal::Bool(false)));
        let string_literal = none_of('"')
            .repeated()
            .delimited_by(just('"'), just('"'))
            .collect()
            .map(Literal::String);

        let literal = choice((
            float,
            integer,
            char_literal,
            bool_literal,
            string_literal,
        )).map(Expression::Literal);

        let atom = literal.or(expr.delimited_by(just('('), just(')')));

        let op = |c| just(c).padded();

        let mul_div = op('*').to(BinaryOp::Mul).or(op('/').to(BinaryOp::Div));
        let add_sub = op('+').to(BinaryOp::Add).or(op('-').to(BinaryOp::Sub));

        let product = atom.clone()
            .then(mul_div.then(atom).repeated())
            .foldl(|lhs, (op, rhs)| Expression::Binary(Box::new(lhs), op, Box::new(rhs)));

        let sum = product.clone()
            .then(add_sub.then(product).repeated())
            .foldl(|lhs, (op, rhs)| Expression::Binary(Box::new(lhs), op, Box::new(rhs)));

        sum
    })
}

fn statement_parser() -> impl Parser<char, Statement, Error = Simple<char>> {
    let let_declaration = just("let").to(false)
        .or(just("mut").to(true))
        .padded()
        .then(text::ident().padded())
        .then(just(':').padded().ignore_then(type_parser()).or_not())
        .then_ignore(just('=').padded())
        .then(expression_parser())
        .then_ignore(just(';'))
        .map(|(((mutable, name), type_info), value)| {
            Statement::Let(LetDeclaration {
                name,
                mutable,
                type_info,
                value,
            })
        });

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
    use crate::ast::{BinaryOp, Expression, Function, LetDeclaration, Literal, Node, Statement, Type};

    #[test]
    fn test_parse_arithmetic_expressions() {
        let source = r#"
            fn main(): void {
                let a = 1 + 2 * 3;
            }
        "#;
        let expected_ast = vec![Node::Function(Function {
            name: "main".to_string(),
            parameters: vec![],
            return_type: Type::Simple("void".to_string()),
            body: vec![Statement::Let(LetDeclaration {
                name: "a".to_string(),
                mutable: false,
                type_info: None,
                value: Expression::Binary(
                    Box::new(Expression::Literal(Literal::Int(1))),
                    BinaryOp::Add,
                    Box::new(Expression::Binary(
                        Box::new(Expression::Literal(Literal::Int(2))),
                        BinaryOp::Mul,
                        Box::new(Expression::Literal(Literal::Int(3))),
                    )),
                ),
            })],
        })];
        assert_eq!(parse(source).unwrap(), expected_ast);
    }
}
