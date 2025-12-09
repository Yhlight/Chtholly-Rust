#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

void run(const std::string& source) {
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();

    if (parser.hadError()) {
        return;
    }

    try {
        Chtholly::SemanticAnalyzer analyzer;
        analyzer.analyze(statements);
        std::cout << "Analysis successful." << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Semantic Error: " << e.what() << std::endl;
    }
}

void runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open file: " << path << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: chtholly [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        std::cout << "Usage: chtholly [script]" << std::endl;
    }
    return 0;
}
