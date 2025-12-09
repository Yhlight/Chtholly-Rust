#include "lexer.h"
#include <unordered_map>
#include <stdexcept>

namespace Chtholly
{
    const std::unordered_map<std::string, TokenType> Lexer::keywords = {
        {"fn", TokenType::FN},
        {"let", TokenType::LET},
        {"mut", TokenType::MUT},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"switch", TokenType::SWITCH},
        {"case", TokenType::CASE},
        {"default", TokenType::DEFAULT},
        {"for", TokenType::FOR},
        {"while", TokenType::WHILE},
        {"do", TokenType::DO},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"return", TokenType::RETURN},
        {"class", TokenType::CLASS},
        {"struct", TokenType::STRUCT},
        {"enum", TokenType::ENUM},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"self", TokenType::SELF},
        {"public", TokenType::PUBLIC},
        {"private", TokenType::PRIVATE},
        {"contract", TokenType::CONTRACT},
        {"require", TokenType::REQUIRE},
        {"fallthrough", TokenType::FALLTHROUGH},
        {"as", TokenType::AS},
        {"import", TokenType::IMPORT},
        {"use", TokenType::USE},
        {"package", TokenType::PACKAGE},
        {"i8", TokenType::I8},
        {"i16", TokenType::I16},
        {"i32", TokenType::I32},
        {"i64", TokenType::I64},
        {"u8", TokenType::U8},
        {"u16", TokenType::U16},
        {"u32", TokenType::U32},
        {"u64", TokenType::U64},
        {"f32", TokenType::F32},
        {"f64", TokenType::F64},
        {"void", TokenType::VOID},
        {"bool", TokenType::BOOL},
    };

    Lexer::Lexer(const std::string& source) : source(source) {}

    std::vector<Token> Lexer::scanTokens()
    {
        while (current < source.length())
        {
            start = current;
            scanToken();
        }
        tokens.push_back({TokenType::END_OF_FILE, "", std::monostate{}, line});
        return tokens;
    }

    void Lexer::scanToken()
    {
        char c = advance();
        switch (c)
        {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-':
            if (match('=')) { addToken(TokenType::MINUS_EQUAL); }
            else if (match('-')) { addToken(TokenType::MINUS_MINUS); }
            else { addToken(TokenType::MINUS); }
            break;
        case '+':
            if (match('=')) { addToken(TokenType::PLUS_EQUAL); }
            else if (match('+')) { addToken(TokenType::PLUS_PLUS); }
            else { addToken(TokenType::PLUS); }
            break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case ':': addToken(TokenType::COLON); break;
        case '*':
            if (match('=')) { addToken(TokenType::STAR_EQUAL); }
            else { addToken(TokenType::STAR); }
            break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '&': addToken(match('&') ? TokenType::AMPERSAND_AMPERSAND : TokenType::AMPERSAND); break;
        case '|': addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE); break;
        case '^': addToken(TokenType::CARET); break;
        case '~': addToken(TokenType::TILDE); break;
        case '/':
            if (match('/'))
            {
                while (peek() != '\n' && current < source.length()) advance();
            }
            else if (match('*'))
            {
                while (peek() != '*' || peekNext() != '/')
                {
                    if (current >= source.length())
                    {
                        addToken(TokenType::ERROR, "Unterminated comment.");
                        return;
                    }
                    if (peek() == '\n') line++;
                    advance();
                }

                if (current < source.length() - 1) {
                    advance(); // Consume '*'
                    advance(); // Consume '/'
                }
            }
            else if (match('=')) { addToken(TokenType::SLASH_EQUAL); }
            else { addToken(TokenType::SLASH); }
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;
        case '"': stringLiteral(); break;
        default:
            if (isdigit(c)) { numberLiteral(); }
            else if (isalpha(c) || c == '_') { identifier(); }
            else {
                addToken(TokenType::ERROR, "Unexpected character.");
            }
            break;
        }
    }

    char Lexer::advance()
    {
        return source[current++];
    }

    bool Lexer::match(char expected)
    {
        if (current >= source.length()) return false;
        if (source[current] != expected) return false;
        current++;
        return true;
    }

    char Lexer::peek() const
    {
        if (current >= source.length()) return '\0';
        return source[current];
    }

    char Lexer::peekNext() const
    {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }

    void Lexer::addToken(TokenType type)
    {
        addToken(type, std::monostate{});
    }

    void Lexer::addToken(TokenType type, const std::variant<std::monostate, int, double, std::string, char>& literal)
    {
        std::string text = source.substr(start, current - start);
        tokens.push_back({type, text, literal, line});
    }

    void Lexer::stringLiteral()
    {
        while (peek() != '"' && current < source.length())
        {
            if (peek() == '\n') line++;
            advance();
        }

        if (current >= source.length())
        {
            addToken(TokenType::ERROR, "Unterminated string.");
            return;
        }

        advance();

        std::string value = source.substr(start + 1, current - start - 2);
        addToken(TokenType::STRING, value);
    }

    void Lexer::numberLiteral()
    {
        while (isdigit(peek())) advance();

        if (peek() == '.' && isdigit(peekNext()))
        {
            advance();
            while (isdigit(peek())) advance();
        }

        if (peek() == 'e' || peek() == 'E')
        {
            advance();
            if (peek() == '+' || peek() == '-')
            {
                advance();
            }
            if (!isdigit(peek())) {
                addToken(TokenType::ERROR, "Malformed number literal.");
                return;
            }
            while (isdigit(peek())) advance();
        }

        std::string numStr = source.substr(start, current - start);
        bool isFloat = numStr.find('.') != std::string::npos || numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos;

        if (isFloat)
        {
            try {
                addToken(TokenType::FLOAT, std::stod(numStr));
            } catch (const std::out_of_range& oor) {
                addToken(TokenType::ERROR, "Float literal out of range.");
            }
        }
        else
        {
            try {
                addToken(TokenType::INTEGER, std::stoi(numStr));
            } catch (const std::out_of_range& oor) {
                addToken(TokenType::ERROR, "Integer literal out of range.");
            }
        }
    }

    void Lexer::identifier()
    {
        while (isalnum(peek()) || peek() == '_') advance();

        std::string text = source.substr(start, current - start);
        auto it = keywords.find(text);
        if (it == keywords.end())
        {
            addToken(TokenType::IDENTIFIER);
        }
        else
        {
            addToken(it->second);
        }
    }

} // namespace Chtholly
