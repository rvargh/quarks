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
    if_
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
};

class Tokenizer {
  public:
    explicit Tokenizer(std::string source) : m_src(std::move(source)) {}

    vector<Token> tokenize() {
        string buffer;
        vector<Token> tokens;

        while (peek().has_value()) {
            if (isalpha(peek().value())) {
                buffer.push_back(eat());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buffer.push_back(eat());
                }
                if (buffer == "exit") {
                    tokens.push_back(
                        {.type = TokenType::exit, .value = buffer});
                    buffer.clear();
                } else if (buffer == "assign") {
                    tokens.push_back({.type = TokenType::assign});
                    buffer.clear();
                } else if (buffer == "if") {
                    tokens.push_back({.type = TokenType::if_});
                    buffer.clear();
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
            } else if (isdigit(peek().value())) {
                buffer.push_back(eat());
                while (peek().has_value() && isdigit(peek().value())) {
                    buffer.push_back(eat());
                }
                tokens.push_back(
                    {.type = TokenType::intLiteral, .value = buffer});
                buffer.clear();
            } else if (peek().value() == '-' && peek(1).has_value() && peek(1).value() == '-') {
                eat();
                eat();
                while (peek().has_value() && peek().value() != '\n') {
                    eat();
                }
            } else if (peek().value() == '-' && peek(1).has_value() && peek(1).value() == '*') {
                eat();
                eat();
                while (peek().has_value() && peek().value() == '*' && peek().value() == '-') {
                    break;
                }
                if (peek().has_value()) {
                    eat();
                }
                if (peek().has_value()) {
                    eat();
                }
            }
            else if (peek().value() == ';') {
                eat();
                tokens.push_back({.type = TokenType::semicolon});
            } else if (peek().value() == '=') {
                eat();
                tokens.push_back({.type = TokenType::equals});
            } else if (isspace(peek().value())) {
                eat();
            } else if (peek().value() == '+') {
                eat();
                tokens.push_back({.type = TokenType::addition});
            } else if (peek().value() == '*') {
                eat();
                tokens.push_back({.type = TokenType::multiplication});
            } else if (peek().value() == '-') {
                eat();
                tokens.push_back({.type = TokenType::substraction});
            } else if (peek().value() == '/') {
                eat();
                tokens.push_back({.type = TokenType::division});
            } else if (peek().value() == '{') {
                eat();
                tokens.push_back({.type = TokenType::open_curly});
            } else if (peek().value() == '}') {
                eat();
                tokens.push_back({.type = TokenType::close_curly});
            } else {
                cerr << "You messed up!" << endl;
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