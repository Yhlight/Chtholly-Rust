#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

int main() {
    const char* source = "fn main() { let x = 10; x; }";
    chtholly::Lexer lexer(source);
    chtholly::Parser parser(lexer);

    // Parse the function definition.
    auto ast = parser.handleDefinition();
    if (!ast) {
        fprintf(stderr, "Error parsing function definition.\n");
        return 1;
    }

    // Generate the LLVM IR.
    chtholly::CodeGen codegen;
    codegen.generate(*ast);

    // Print the generated IR.
    codegen.getModule().print(llvm::errs(), nullptr);

    return 0;
}
