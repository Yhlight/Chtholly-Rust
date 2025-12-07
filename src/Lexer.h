#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace Chtholly {

class Lexer {
public:
  Lexer(const std::string& source);
  std::vector<Token> scanTokens();

private:
  void scanToken();
  char advance();
  void addToken(TokenType type);
  bool match(char expected);
  char peek();
  char peekNext();
  void string();
  void number();
  void identifier();
  void blockComment();

  const std::string m_source;
  std::vector<Token> m_tokens;
  int m_start = 0;
  int m_current = 0;
  int m_line = 1;

  static const std::unordered_map<std::string, TokenType> m_keywords;
};

} // namespace Chtholly

#endif // CHTHOLLY_LEXER_H
