#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::string& code) : code_(code), pos_(0) {
    // Constructor
}

void Parser::parse() {
    while (pos_ < code_.length()) {
        if (code_[pos_] == '/' && pos_ + 1 < code_.length()) {
            if (code_[pos_ + 1] == '/') {
                // Single-line comment
                pos_ += 2;
                while (pos_ < code_.length() && code_[pos_] != '\n') {
                    pos_++;
                }
            } else if (code_[pos_ + 1] == '*') {
                // Multi-line comment
                pos_ += 2;
                bool found_end = false;
                while (pos_ + 1 < code_.length()) {
                    if (code_[pos_] == '*' && code_[pos_ + 1] == '/') {
                        pos_ += 2;
                        found_end = true;
                        break;
                    }
                    pos_++;
                }
                if (!found_end) {
                    throw std::runtime_error("Unterminated multi-line comment");
                }
            } else {
                pos_++;
            }
        } else {
            pos_++;
        }
    }
}
