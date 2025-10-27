#pragma once

#include "common.hpp"

enum class TokenType { exit, int_literal, semicolon };

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
  public:
    inline explicit Tokenizer(std::string source) : m_src(std::move(source)) {}

    inline std::vector<Token> tokenize() {
        std::string buffer;
        std::vector<Token> tokens;

        // e x i t   2 1 ;
        // 0 1 2 3 4 5 6 7

        while (peak().has_value()) {
            if (std::isalpha(peak().value())) {
                buffer.push_back(consume());
                while (peak().has_value() && std::isalnum(peak().value())) {
                    buffer.push_back(consume());
                }

                if (buffer == "exit") {
                    tokens.push_back(
                        {.type = TokenType::exit, .value = buffer});
                    buffer.clear();
                    continue;
                } else {
                    std::cerr << "You messed up!" << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else if (std::isdigit(peak().value())) {
                buffer.push_back(consume());
                while (peak().has_value() && std::isdigit(peak().value())) {
                    buffer.push_back(consume());
                }
                tokens.push_back(
                    {.type = TokenType::int_literal, .value = buffer});
                buffer.clear();
            } else if (peak().value() == ';') {
                consume();
                tokens.push_back({.type = TokenType::semicolon});
            } else if (std::isspace(peak().value())) {
                consume();
                continue;
            } else {
                std::cerr << "You messed up!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        m_index = 0;
        return tokens;
    }

  private:
    [[nodiscard]] std::optional<char> peak(const int ahead = 1) const {
        if (m_index + ahead > m_src.length()) {
            return {};
        } else {
            return m_src.at(m_index);
        }
    }

    char consume() {
        return m_src.at(m_index++);
    }

    const std::string m_src;
    int m_index = 0;
};