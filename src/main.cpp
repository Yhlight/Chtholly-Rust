#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Semantic/SemanticAnalyzer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: chtholly <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        Lexer lexer(source);
        Parser parser(lexer);
        std::unique_ptr<ProgramAST> ast = parser.parse();

        SemanticAnalyzer analyzer;
        analyzer.visit(*ast);

        std::cout << "Parsing and semantic analysis successful!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
