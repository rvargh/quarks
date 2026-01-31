#pragma once

using namespace std;

enum class TokenType {
    exit,
    intLiteral,
    semicolon,
    openParentheses,
    closeParentheses,
    identifier,
    assign,
    equals,
    addition,
    multiplication,
    division,
    substraction,
    open_curly,
    close_curly,
    if_,
    elif,
    else_,
};

inline std::optional<int> isBinaryOperator(const TokenType type) {
    switch (type) {
    case TokenType::substraction:
    case TokenType::addition:
        return 0;
    case TokenType::multiplication:
    case TokenType::division:
        return 1;
    default:
        return nullopt;
    }
}

struct Token {
    TokenType type;
    optional<std::string> value;
    int line;
};

class Tokenizer {
  public:
    explicit Tokenizer(std::string source) : m_src(std::move(source)) {}

    vector<Token> tokenize() {
        string buffer;
        vector<Token> tokens;
        int line_count = 0;

        while (peek().has_value()) {
            if (isalpha(peek().value())) {
                buffer.push_back(eat());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buffer.push_back(eat());
                }
                if (buffer == "exit") {
                    tokens.push_back(
                        {.type = TokenType::exit, .value = buffer, .line = line_count});
                    buffer.clear();
                } else if (buffer == "assign") {
                    tokens.push_back({.type = TokenType::assign, .line = line_count});
                    buffer.clear();
                } else if (buffer == "if") {
                    tokens.push_back({.type = TokenType::if_, .line = line_count});
                    buffer.clear();
                } else if (buffer == "elif") {
                    tokens.push_back({.type = TokenType::elif, .line = line_count});
                    buffer.clear();
                } else if (buffer == "else") {
                    tokens.push_back({.type = TokenType::else_, .line = line_count});
                    buffer.clear();
                } else {
                    tokens.push_back(
                        {.type = TokenType::identifier, .value = buffer});
                    buffer.clear();
                }
            } else if (peek().value() == '(') {
                eat();
                tokens.push_back({.type = TokenType::openParentheses, .line = line_count});
            } else if (peek().value() == ')') {
                eat();
                tokens.push_back({.type = TokenType::closeParentheses, .line = line_count});
            } else if (isdigit(peek().value())) {
                buffer.push_back(eat());
                while (peek().has_value() && isdigit(peek().value())) {
                    buffer.push_back(eat());
                }
                tokens.push_back(
                    {.type = TokenType::intLiteral, .value = buffer, .line = line_count});
                buffer.clear();
            } else if (peek().value() == '-' && peek(1).has_value() &&
                       peek(1).value() == '-') {
                eat();
                eat();
                while (peek().has_value() && peek().value() != '\n') {
                    eat();
                }
            } else if (peek().value() == '-' && peek(1).has_value() &&
                       peek(1).value() == '*') {
                eat();
                eat();
                while (peek().has_value() && peek().value() == '*' &&
                       peek().value() == '-') {
                    break;
                }
                if (peek().has_value()) {
                    eat();
                }
                if (peek().has_value()) {
                    eat();
                }
            } else if (peek().value() == ';') {
                eat();
                tokens.push_back({.type = TokenType::semicolon, .line = line_count});
            } else if (peek().value() == '=') {
                eat();
                tokens.push_back({.type = TokenType::equals, .line = line_count});
            } else if (peek().value() == '+') {
                eat();
                tokens.push_back({.type = TokenType::addition, .line = line_count});
            } else if (peek().value() == '*') {
                eat();
                tokens.push_back({.type = TokenType::multiplication, .line = line_count});
            } else if (peek().value() == '-') {
                eat();
                tokens.push_back({.type = TokenType::substraction, .line = line_count});
            } else if (peek().value() == '/') {
                eat();
                tokens.push_back({.type = TokenType::division, .line = line_count});
            } else if (peek().value() == '{') {
                eat();
                tokens.push_back({.type = TokenType::open_curly, .line = line_count});
            } else if (peek().value() == '}') {
                eat();
                tokens.push_back({.type = TokenType::close_curly, .line = line_count});
            } else if (peek().value() == '\n') {
                eat();
                line_count++;
            } else if (std::isspace(peek().value())) {
                eat();
            } else {
                cerr << "Invalid token" << endl;
                exit(EXIT_FAILURE);
            }
        }

        m_index = 0;
        return tokens;
    }

  private:
    [[nodiscard]] inline optional<char> peek(const int offset = 0) const {
        if (m_index + offset >= m_src.length()) {
            return {};
        } else {
            return m_src.at(m_index + offset);
        }
    }

    char eat() { return m_src.at(m_index++); }

    const string m_src;
    int m_index = 0;
};