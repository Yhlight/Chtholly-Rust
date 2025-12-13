#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "CodeGenerator.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto functions = parser.parse();

    if (!parser.getErrors().empty()) {
        for (const auto& error : parser.getErrors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    Chtholly::SemanticAnalyzer analyzer;
    for (const auto& func : functions) {
        try {
            analyzer.analyze(*func);
        } catch (const std::runtime_error& e) {
            std::cerr << "Semantic error: " << e.what() << std::endl;
            return 1;
        }
    }

    Chtholly::CodeGenerator generator;
    for (const auto& func : functions) {
        generator.generate(*func);
    }

    generator.dump();

    return 0;
}
