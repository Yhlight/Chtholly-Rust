#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

using namespace Chtholly;

TEST(ParserTest, ParsesMainFunction) {
    std::string source = "fn main() {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::unique_ptr<TranslationUnitAST> ast = parser.parse();

    ASSERT_NE(ast, nullptr);
    const auto& functions = ast->getFunctions();
    ASSERT_EQ(functions.size(), 1);
    EXPECT_EQ(functions[0]->getName(), "main");
}
