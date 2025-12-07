// src/parser.rs

use pest::Parser;
use pest_derive::Parser;

#[derive(Parser)]
#[grammar = "chtholly.pest"]
pub struct ChthollyParser;

pub fn parse(input: &str) -> Result<pest::iterators::Pairs<'_, Rule>, pest::error::Error<Rule>> {
    ChthollyParser::parse(Rule::program, input)
}
