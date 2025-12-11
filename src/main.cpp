#include "Lexer.h"
#include "Parser.h"
#include <iostream>

int main() {
    const char* source = "fn main() { 10; }";
    chtholly::Lexer lexer(source);
    chtholly::Parser parser(lexer);

    // For now, we'll just try to parse a function definition
    // and print a message. In a real compiler, this would
    // build up the AST.
    parser.handleDefinition();

    return 0;
}
