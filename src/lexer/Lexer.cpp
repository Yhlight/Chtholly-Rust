#include "Lexer.h"

#include <utility>
#include <unordered_map>
#include <cctype>

namespace Chtholly
{
    const std::unordered_map<std::string, TokenType> keywords = {
        {"let", TokenType::Let},
        {"mut", TokenType::Mut},
        {"fn", TokenType::Fn},
        {"return", TokenType::Return},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"while", TokenType::While},
        {"for", TokenType::For},
        {"class", TokenType::Class},
        {"struct", TokenType::Struct},
        {"enum", TokenType::Enum},
    };

    Lexer::Lexer(std::string source) : m_source(std::move(source)) {}

    std::vector<Token> Lexer::Tokenize()
    {
        while (!IsAtEnd())
        {
            m_start = m_current;
            ScanToken();
        }

        m_tokens.emplace_back(TokenType::Eof, "");
        return m_tokens;
    }

    void Lexer::ScanToken()
    {
        char c = Advance();
        switch (c)
        {
            case '(': AddToken(TokenType::LParen); break;
            case ')': AddToken(TokenType::RParen); break;
            case '{': AddToken(TokenType::LBrace); break;
            case '}': AddToken(TokenType::RBrace); break;
            case ',': AddToken(TokenType::Comma); break;
            case '.': AddToken(TokenType::Unknown); break;
            case '-': AddToken(Match('=') ? TokenType::MinusEqual : TokenType::Minus); break;
            case '+': AddToken(Match('=') ? TokenType::PlusEqual : TokenType::Plus); break;
            case ';': AddToken(TokenType::Semicolon); break;
            case '*': AddToken(Match('=') ? TokenType::AsteriskEqual : TokenType::Asterisk); break;
            case '!': AddToken(Match('=') ? TokenType::BangEqual : TokenType::Bang); break;
            case '=': AddToken(Match('=') ? TokenType::DoubleEqual : TokenType::Equal); break;
            case '<': AddToken(Match('=') ? TokenType::LessEqual : TokenType::Less); break;
            case '>': AddToken(Match('=') ? TokenType::GreaterEqual : TokenType::Greater); break;
            case '/':
                if (Match('/'))
                {
                    while (Peek() != '\n' && !IsAtEnd()) Advance();
                }
                else if (Match('*'))
                {
                    while (!(Peek() == '*' && PeekNext() == '/') && !IsAtEnd())
                    {
                        Advance();
                    }
                    if (!IsAtEnd())
                    {
                        Advance();
                        Advance();
                    }
                }
                else
                {
                    AddToken(Match('=') ? TokenType::SlashEqual : TokenType::Slash);
                }
                break;
            case '&':
                if (Match('&'))
                {
                    AddToken(TokenType::DoubleAmpersand);
                }
                break;
            case '|':
                if (Match('|'))
                {
                    AddToken(TokenType::DoublePipe);
                }
                break;

            case ' ':
            case '\r':
            case '\t':
            case '\n':
                break;

            case '"': String(); break;
            case '\'': Char(); break;

            default:
                if (std::isdigit(c))
                {
                    Number();
                }
                else if (std::isalpha(c))
                {
                    Identifier();
                }
                else
                {
                    AddToken(TokenType::Unknown);
                }
                break;
        }
    }

    void Lexer::Identifier()
    {
        while (std::isalnum(Peek())) Advance();
        std::string text = m_source.substr(m_start, m_current - m_start);
        auto it = keywords.find(text);
        if (it != keywords.end())
        {
            AddToken(it->second);
        }
        else
        {
            AddToken(TokenType::Identifier);
        }
    }

    void Lexer::Number()
    {
        while (std::isdigit(Peek())) Advance();
        if (Peek() == '.' && std::isdigit(PeekNext()))
        {
            Advance();
            while (std::isdigit(Peek())) Advance();
            AddToken(TokenType::Float);
        }
        else
        {
            AddToken(TokenType::Integer);
        }
    }

    void Lexer::String()
    {
        while (Peek() != '"' && !IsAtEnd())
        {
            Advance();
        }

        if (IsAtEnd())
        {
            // Unterminated string.
            AddToken(TokenType::Unknown);
            return;
        }

        Advance(); // The closing ".

        // Trim the surrounding quotes.
        std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
        m_tokens.emplace_back(TokenType::String, value);
    }

    void Lexer::Char()
    {
        // Check for unterminated or empty char literal
        if (Peek() == '\'' || IsAtEnd())
        {
            AddToken(TokenType::Unknown);
            if (!IsAtEnd()) Advance();
            return;
        }

        char value = Advance();

        if (Peek() != '\'')
        {
            // Multi-character literal
            while(Peek() != '\'' && !IsAtEnd()) Advance();
            if (!IsAtEnd()) Advance();
            AddToken(TokenType::Unknown);
            return;
        }

        Advance(); // The closing '.

        m_tokens.emplace_back(TokenType::Char, std::string(1, value));
    }

    char Lexer::Peek()
    {
        if (IsAtEnd()) return '\0';
        return m_source[m_current];
    }

    char Lexer::PeekNext()
    {
        if (m_current + 1 >= m_source.length()) return '\0';
        return m_source[m_current + 1];
    }

    bool Lexer::IsAtEnd()
    {
        return m_current >= m_source.length();
    }

    char Lexer::Advance()
    {
        m_current++;
        return m_source[m_current - 1];
    }

    void Lexer::AddToken(TokenType type)
    {
        std::string text = m_source.substr(m_start, m_current - m_start);
        m_tokens.emplace_back(type, text);
    }

    bool Lexer::Match(char expected)
    {
        if (IsAtEnd()) return false;
        if (m_source[m_current] != expected) return false;
        m_current++;
        return true;
    }

}
