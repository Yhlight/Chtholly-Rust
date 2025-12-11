use crate::ast::{BinaryOp, Expression, LiteralValue, Statement, Type};
use nom::{
    branch::alt,
    bytes::complete::{tag, take_until},
    character::complete::{alpha1, char, digit1, multispace0},
    combinator::{map, map_res, opt, recognize},
    multi::fold_many0,
    sequence::{delimited, preceded, tuple},
    IResult,
};

// Parse single-line comments
fn parse_single_line_comment(input: &str) -> IResult<&str, &str> {
    let (input, _) = tag("//")(input)?;
    let (input, comment) = take_until("\n")(input)?;
    Ok((input, comment))
}

// Parse multi-line comments
fn parse_multi_line_comment(input: &str) -> IResult<&str, &str> {
    let (input, _) = tag("/*")(input)?;
    let (input, comment) = take_until("*/")(input)?;
    let (input, _) = tag("*/")(input)?;
    Ok((input, comment))
}

// Parse either a single-line or multi-line comment
pub fn parse_comment(input: &str) -> IResult<&str, &str> {
    alt((parse_single_line_comment, parse_multi_line_comment))(input)
}

// Parse the main function
pub fn parse_main_function(input: &str) -> IResult<&str, &str> {
    let (input, _) = tuple((
        multispace0,
        tag("fn"),
        multispace0,
        tag("main"),
        multispace0,
        char('('),
        multispace0,
        tag("args"),
        multispace0,
        char(':'),
        multispace0,
        tag("string[]"),
        multispace0,
        char(')'),
        multispace0,
        char(':'),
        multispace0,
        tag("Result<i32, SystemError>"),
        multispace0,
    ))(input)?;

    let (input, body) = delimited(
        tuple((char('{'), multispace0)),
        take_until("}"),
        tuple((multispace0, char('}'))),
    )(input)?;

    Ok((input, body))
}

/// Parses a primitive type into a `Type` enum.
pub fn parse_type(input: &str) -> IResult<&str, Type> {
    alt((
        map(tag("i32"), |_| Type::I32),
        map(tag("bool"), |_| Type::Bool),
    ))(input)
}

/// Parses a boolean literal (`true` or `false`).
fn parse_bool(input: &str) -> IResult<&str, LiteralValue> {
    alt((
        map(tag("true"), |_| LiteralValue::Boolean(true)),
        map(tag("false"), |_| LiteralValue::Boolean(false)),
    ))(input)
}

/// Parses an integer literal.
fn parse_integer(input: &str) -> IResult<&str, LiteralValue> {
    map(map_res(recognize(digit1), str::parse), |val| {
        LiteralValue::Integer(val)
    })(input)
}

/// Parses any literal value into an `Expression`.
pub fn parse_literal(input: &str) -> IResult<&str, Expression> {
    map(alt((parse_integer, parse_bool)), |literal| {
        Expression::Literal(literal)
    })(input)
}

/// Parses an identifier.
pub fn parse_identifier(input: &str) -> IResult<&str, Expression> {
    map(alpha1, |s: &str| Expression::Identifier(s.to_string()))(input)
}

/// Parses factors, which are the highest-precedence expressions.
/// factor = literal | identifier | "(" expression ")"
fn parse_factor(input: &str) -> IResult<&str, Expression> {
    delimited(
        multispace0,
        alt((
            parse_literal,
            parse_identifier,
            delimited(char('('), parse_expression, char(')')),
        )),
        multispace0,
    )(input)
}

/// Parses terms, handling multiplication, division, and modulo.
/// term = factor { ("*" | "/" | "%") factor }
fn parse_term(input: &str) -> IResult<&str, Expression> {
    let (input, left) = parse_factor(input)?;

    fold_many0(
        tuple((
            alt((
                map(tag("*"), |_| BinaryOp::Multiply),
                map(tag("/"), |_| BinaryOp::Divide),
                map(tag("%"), |_| BinaryOp::Modulo),
            )),
            parse_factor,
        )),
        move || left.clone(),
        |acc, (op, right)| Expression::Binary {
            op,
            left: Box::new(acc),
            right: Box::new(right),
        },
    )(input)
}

/// Parses expressions, handling addition and subtraction.
/// expression = term { ("+" | "-") term }
pub fn parse_expression(input: &str) -> IResult<&str, Expression> {
    let (input, left) = parse_term(input)?;

    fold_many0(
        tuple((
            alt((
                map(tag("+"), |_| BinaryOp::Add),
                map(tag("-"), |_| BinaryOp::Subtract),
            )),
            parse_term,
        )),
        move || left.clone(),
        |acc, (op, right)| Expression::Binary {
            op,
            left: Box::new(acc),
            right: Box::new(right),
        },
    )(input)
}

/// Parses a `let` or `let mut` statement.
pub fn parse_let_statement(input: &str) -> IResult<&str, Statement> {
    let (input, _) = tag("let")(input)?;
    let (input, _) = multispace0(input)?;
    let (input, is_mutable) = map(opt(tag("mut")), |m| m.is_some())(input)?;
    let (input, _) = multispace0(input)?;
    let (input, name) = alpha1(input)?;
    let (input, _) = multispace0(input)?;
    let (input, type_annotation) = opt(preceded(
        tuple((char(':'), multispace0)),
        parse_type,
    ))(input)?;
    let (input, _) = multispace0(input)?;
    let (input, _) = char('=')(input)?;
    let (input, _) = multispace0(input)?;
    let (input, value) = parse_expression(input)?;
    let (input, _) = char(';')(input)?;

    Ok((
        input,
        Statement::Let {
            name: name.to_string(),
            is_mutable,
            type_annotation,
            value,
        },
    ))
}
