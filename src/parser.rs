use pest_derive::Parser;

#[derive(Parser)]
#[grammar = "chtholly.pest"]
pub struct ChthollyParser;
