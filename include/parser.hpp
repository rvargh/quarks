//
// Created by royva on 27-10-2025.
//
#pragma once

// these are just tokens(integer literals)
struct ExpressionNode {
    Token int_literals;
};

// inside the exit node we have the expression
struct ExitNode {
    ExpressionNode expr;
};

struct intLiteral {

};

class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
        :m_tokens(std::move(tokens)){}

    std::optional<ExpressionNode> parseExpression() {
        if (peak().has_value() && peak().value().type == TokenType::intLiteral) {
            return ExpressionNode {.int_literals = consume()};
        } else {
            return std::nullopt;
        }
    }

    std::optional<ExitNode> parse() {
        std::optional<ExitNode> exit_node;
        while(peak().has_value()) {
            if (peak().value().type == TokenType::exit) {
                consume();
                if (auto node_expression = parseExpression()) {
                    exit_node = ExitNode {.expr = node_expression.value()};
                } else {
                    std::cerr << "Invalid expression pass" << std::endl;
                    exit(EXIT_FAILURE);

                }
            }

            if (peak().has_value() && peak().value().type == TokenType::semicolon) {
                consume();
            } else {
                std::cerr << "Invalid expression pass" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return exit_node;
    }


private:
    const std::vector<Token> m_tokens;

    [[nodiscard]] inline std::optional<Token> peak(const int ahead = 1) const {
        if (m_index + ahead > m_tokens.size()) {
            return {};
        } else {
            return m_tokens.at(m_index);
        }
    }

    Token consume() {
        return m_tokens.at(m_index++);
    }

    size_t m_index = 0;
};


