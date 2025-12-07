#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include <string>

class Parser {
public:
    explicit Parser(const std::string& code);
    void parse();

private:
    std::string code_;
    size_t pos_;
};

#endif //CHTHOLLY_PARSER_H
