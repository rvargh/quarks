#pragma once

enum class TokenType {
    exit,
    intLiteral,
    semicolon,
    openParentheses,
    closeParentheses,
    identifier,
    assign,
    equals
};

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

        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buffer.push_back(eat());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buffer.push_back(eat());
                }
                if (buffer == "exit") {
                    tokens.push_back(
                        {.type = TokenType::exit, .value = buffer});
                    buffer.clear();
                    continue;
                } else if (buffer == "assign") {
                    tokens.push_back({.type = TokenType::assign});
                    buffer.clear();
                    continue;
                } else {
                    tokens.push_back(
                        {.type = TokenType::identifier, .value = buffer});
                    buffer.clear();
                }
            } else if (peek().value() == '(') {
                eat();
                tokens.push_back({.type = TokenType::openParentheses});
            } else if (peek().value() == ')') {
                eat();
                tokens.push_back({.type = TokenType::closeParentheses});
            } else if (std::isdigit(peek().value())) {
                buffer.push_back(eat());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buffer.push_back(eat());
                }
                tokens.push_back(
                    {.type = TokenType::intLiteral, .value = buffer});
                buffer.clear();
            } else if (peek().value() == ';') {
                eat();
                tokens.push_back({.type = TokenType::semicolon});
            } else if (peek().value() == '=') {
                eat();
                tokens.push_back({.type = TokenType::equals});
            } else if (std::isspace(peek().value())) {
                eat();
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
    [[nodiscard]] inline std::optional<char> peek(const int offset = 0) const {
        if (m_index + offset >= m_src.length()) {
            return {};
        } else {
            return m_src.at(m_index + offset);
        }
    }

    char eat() { return m_src.at(m_index++); }

    const std::string m_src;
    int m_index = 0;
};