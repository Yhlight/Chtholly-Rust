#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include <vector>
#include <memory>

namespace Chtholly {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);

    std::unique_ptr<TranslationUnitAST> parse();

private:
    std::unique_ptr<FunctionDeclAST> parseFunction();

    const Token& peek();
    const Token& advance();
    bool match(TokenType type);

    const std::vector<Token>& tokens;
    size_t current = 0;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
