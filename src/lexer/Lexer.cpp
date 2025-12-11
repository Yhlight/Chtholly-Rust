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
        while (m_current < m_source.length())
        {
            char c = m_source[m_current];

            if (std::isspace(c))
            {
                m_current++;
                continue;
            }

            if (std::isalpha(c))
            {
                int start = m_current;
                while (m_current < m_source.length() && std::isalnum(m_source[m_current]))
                {
                    m_current++;
                }
                std::string text = m_source.substr(start, m_current - start);
                auto it = keywords.find(text);
                if (it != keywords.end())
                {
                    m_tokens.emplace_back(it->second, text);
                }
                else
                {
                    m_tokens.emplace_back(TokenType::Identifier, text);
                }
                continue;
            }

            if (std::isdigit(c))
            {
                int start = m_current;
                while (m_current < m_source.length() && std::isdigit(m_source[m_current]))
                {
                    m_current++;
                }
                std::string text = m_source.substr(start, m_current - start);
                m_tokens.emplace_back(TokenType::Integer, text);
                continue;
            }

            switch (c)
            {
                case '=':
                    m_tokens.emplace_back(TokenType::Equal, "=");
                    break;
                case ';':
                    m_tokens.emplace_back(TokenType::Semicolon, ";");
                    break;
                default:
                    m_tokens.emplace_back(TokenType::Unknown, std::string(1, c));
                    break;
            }
            m_current++;
        }

        return m_tokens;
    }
}
