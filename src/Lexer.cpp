#include "Lexer.h"

#include <cctype>

namespace Chtholly {

const std::unordered_map<std::string, TokenType> Lexer::m_keywords = {
    {"fn", TokenType::FN},
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"class", TokenType::CLASS},
    {"struct", TokenType::STRUCT},
    {"enum", TokenType::ENUM},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"fallthrough", TokenType::FALLTHROUGH},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"foreach", TokenType::FOREACH},
    {"do", TokenType::DO},
    {"return", TokenType::RETURN},
    {"public", TokenType::PUBLIC},
    {"private", TokenType::PRIVATE},
    {"import", TokenType::IMPORT},
    {"use", TokenType::USE},
    {"as", TokenType::AS},
    {"package", TokenType::PACKAGE},
    {"require", TokenType::REQUIRE},
    {"type", TokenType::TYPE},
    {"Self", TokenType::SELF},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
};

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::scanTokens() {
  while (m_current < m_source.length()) {
    m_start = m_current;
    scanToken();
  }

  m_tokens.push_back({TokenType::END_OF_FILE, "", m_line});
  return m_tokens;
}

void Lexer::scanToken() {
  char c = advance();
  switch (c) {
    case '(': addToken(TokenType::LEFT_PAREN); break;
    case ')': addToken(TokenType::RIGHT_PAREN); break;
    case '{': addToken(TokenType::LEFT_BRACE); break;
    case '}': addToken(TokenType::RIGHT_BRACE); break;
    case '[': addToken(TokenType::LEFT_BRACKET); break;
    case ']': addToken(TokenType::RIGHT_BRACKET); break;
    case ',': addToken(TokenType::COMMA); break;
    case '.': addToken(TokenType::DOT); break;
    case '-':
        if (match('-')) {
            addToken(TokenType::MINUS_MINUS);
        } else if (match('=')) {
            addToken(TokenType::MINUS_EQUAL);
        } else {
            addToken(TokenType::MINUS);
        }
        break;
    case '+':
        if (match('+')) {
            addToken(TokenType::PLUS_PLUS);
        } else if (match('=')) {
            addToken(TokenType::PLUS_EQUAL);
        } else {
            addToken(TokenType::PLUS);
        }
        break;
    case ';': addToken(TokenType::SEMICOLON); break;
    case '*':
        if (match('=')) {
            addToken(TokenType::STAR_EQUAL);
        } else {
            addToken(TokenType::STAR);
        }
        break;
    case '%':
        if (match('=')) {
            addToken(TokenType::MODULO_EQUAL);
        } else {
            addToken(TokenType::MODULO);
        }
        break;
    case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
    case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
    case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
    case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
    case '/':
      if (match('/')) {
        // A comment goes until the end of the line.
        while (peek() != '\n' && m_current < m_source.length()) advance();
      } else if (match('*')) {
        blockComment();
      } else if (match('=')) {
          addToken(TokenType::SLASH_EQUAL);
      }
      else {
        addToken(TokenType::SLASH);
      }
      break;
    case '&':
        if (match('&')) {
            addToken(TokenType::AMPERSAND_AMPERSAND);
        } else {
            addToken(TokenType::AMPERSAND);
        }
        break;
    case '|':
        if (match('|')) {
            addToken(TokenType::PIPE_PIPE);
        } else {
            addToken(TokenType::PIPE);
        }
        break;
    case ':':
        if (match(':')) {
            addToken(TokenType::COLON_COLON);
        } else {
            addToken(TokenType::COLON);
        }
        break;

    case ' ':
    case '\r':
    case '\t':
      // Ignore whitespace.
      break;

    case '\n':
      m_line++;
      break;

    case '"': string(); break;
    case '\'':
        if (m_current + 1 < m_source.length() && m_source[m_current + 1] == '\'') {
            m_current++; // consume character
            m_current++; // consume closing quote
            addToken(TokenType::CHARACTER);
        }
        break;

    default:
      if (isdigit(c)) {
        number();
      } else if (isalpha(c) || c == '_') {
        identifier();
      }
      else {
        // Handle error, for now just skip the character
      }
      break;
  }
}

char Lexer::advance() {
  return m_source[m_current++];
}

void Lexer::addToken(TokenType type) {
  std::string text = m_source.substr(m_start, m_current - m_start);
  m_tokens.push_back({type, text, m_line});
}

bool Lexer::match(char expected) {
  if (m_current >= m_source.length()) return false;
  if (m_source[m_current] != expected) return false;

  m_current++;
  return true;
}

char Lexer::peek() {
  if (m_current >= m_source.length()) return '\0';
  return m_source[m_current];
}

char Lexer::peekNext() {
    if (m_current + 1 >= m_source.length()) return '\0';
    return m_source[m_current + 1];
}

void Lexer::string() {
  while (peek() != '"' && m_current < m_source.length()) {
    if (peek() == '\n') m_line++;
    advance();
  }

  if (m_current >= m_source.length()) {
    // Unterminated string.
    return;
  }

  // The closing ".
  advance();

  addToken(TokenType::STRING);
}

void Lexer::number() {
  while (isdigit(peek())) advance();

  // Look for a fractional part.
  if (peek() == '.' && isdigit(peekNext())) {
    // Consume the "."
    advance();

    while (isdigit(peek())) advance();
  }

  addToken(TokenType::NUMBER);
}

void Lexer::identifier() {
  while (isalnum(peek()) || peek() == '_') advance();

  std::string text = m_source.substr(m_start, m_current - m_start);
  TokenType type = TokenType::IDENTIFIER;
  auto it = m_keywords.find(text);
  if (it != m_keywords.end()) {
      type = it->second;
  }
  addToken(type);
}

void Lexer::blockComment() {
    while (m_current + 1 < m_source.length() && (m_source[m_current] != '*' || m_source[m_current + 1] != '/')) {
        if (m_source[m_current] == '\n') {
            m_line++;
        }
        advance();
    }
    if (m_current + 1 < m_source.length()) {
        advance(); // Consume '*'
        advance(); // Consume '/'
    } else {
        // Unterminated block comment
    }
}

} // namespace Chtholly
