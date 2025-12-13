#include "CodeGenerator.h"
#include "Parser.h"
#include "Lexer.h"
#include "SemanticAnalyzer.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <string>
#include <vector>

using namespace Chtholly;

bool runTest(const std::string& testName, const std::string& source, const std::vector<std::string>& expected) {
    std::cout << "Running test: " << testName << "..." << std::endl;
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    analyzer.analyze(functions);
    CodeGenerator generator;
    for (auto& func : functions) {
        generator.generate(*func);
    }
    std::string ir;
    llvm::raw_string_ostream os(ir);
    generator.dump(os);

    for (const auto& exp : expected) {
        if (ir.find(exp) == std::string::npos) {
            std::cerr << "Failed: " << testName << std::endl;
            std::cerr << "  Expected to find: " << exp << std::endl;
            std::cerr << "  In IR:\n" << ir << std::endl;
            return false;
        }
    }

    std::cout << "Passed: " << testName << std::endl;
    return true;
}

int main() {
    int failed = 0;

    if (!runTest("WhileLoop", "fn main(): void { let i: i32 = 0; while (i < 10) { i = i + 1; } }", {"while.cond", "while.body", "while.end"})) {
        failed++;
    }

    if (!runTest("ForLoop", "fn main(): void { for (let i: i32 = 0; i < 10; i = i + 1) {} }", {"for.cond", "for.body", "for.end"})) {
        failed++;
    }

    if (!runTest("DoWhileLoop", "fn main(): void { let i: i32 = 0; do { i = i + 1; } while (i < 10); }", {"do.body", "do.cond", "do.end"})) {
        failed++;
    }

    if (!runTest("Switch", "fn main(): void { let x: i32 = 1; switch (x) { case 1: {} case 2: {} } }", {"switch.end", "switch.case"})) {
        failed++;
    }

    if (failed > 0) {
        std::cerr << failed << " tests failed." << std::endl;
        return 1;
    }

    std::cout << "All code generator tests passed." << std::endl;
    return 0;
}
