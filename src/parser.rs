//! Parser for the Chtholly language.

use pest::Parser;
use crate::ast::{Program, Statement, MainFunction};

#[derive(Parser)]
#[grammar = "chtholly.pest"]
pub struct ChthollyParser;

/// Parses a Chtholly source string and returns the AST.
pub fn parse(source: &str) -> Result<Program, pest::error::Error<Rule>> {
    let pairs = ChthollyParser::parse(Rule::program, source)?;
    let mut program = Program { body: Vec::new() };

    for pair in pairs.flatten() {
        match pair.as_rule() {
            Rule::main_function => {
                program.body.push(Statement::MainFunction(MainFunction));
            }
            _ => (),
        }
    }

    Ok(program)
}
