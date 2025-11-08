//
// Created by royva on 27-10-2025.
//
#pragma once

struct ExpressionIntLiteralNode {
    Token int_literals;
};

struct ExpressionIdentifierNode {
    Token identifier;
};

// these are just tokens(integer literals)
struct ExpressionNode {
    std::variant<ExpressionIntLiteralNode, ExpressionIdentifierNode> var;
};

struct StatementExitNode {
    ExpressionNode expr;
};

struct LetStatementNode {
    Token identidier;
    ExpressionNode expression;
};

struct StatementNode {
    std::variant<StatementExitNode, LetStatementNode> var;
};

struct ProgramNode {
    std::vector<StatementNode> stms;
};

struct intLiteral {};

class Parser {
  public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)) {}

    std::optional<ExpressionNode> parseExpression() {
        if (peek().has_value() &&
            peek().value().type == TokenType::intLiteral) {
            return ExpressionNode{
                .var = ExpressionIntLiteralNode{.int_literals = eat()}};
        } else if (peek().has_value() &&
                   peek().value().type == TokenType::identifier) {
            return ExpressionNode{
                .var = ExpressionIdentifierNode{.identifier = eat()}};
        } else {
            return std::nullopt;
        }
    }

    std::optional<StatementNode> parseStatment() {

        if (peek().value().type == TokenType::exit && peek(1).has_value() &&
            peek(1).value().type == TokenType::openParanthesis) {
            eat();
            eat();
            StatementExitNode statment_exit;
            if (std::optional<ExpressionNode> node_expression = parseExpression()) {
                statment_exit = {.expr = node_expression.value()};
            } else {
                std::cerr << "Invalid expression pass" << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() && peek().value().type == TokenType::closeParanthesis) {
                eat();
            } else {
                std::cerr << "Expected `)`" << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() && peek().value().type == TokenType::semicolon) {
                eat();
            } else {
                std::cerr << "Expected `;`" << std::endl;
                exit(EXIT_FAILURE);
            }

            return StatementNode{.var = statment_exit};
        }

        if (peek().has_value() && peek().value().type == TokenType::assign &&
            peek(1).has_value() &&
            peek(1).value().type == TokenType::identifier &&
            peek(2).has_value() && peek(2).value().type == TokenType::equals) {
            // assign(variable declaration) since we dont need it.
            eat();
            // identifier we eat
            LetStatementNode statment_let =
                LetStatementNode{.identidier = eat()};
            eat();
            if (std::optional<ExpressionNode> node_expression =
                    parseExpression()) {
                statment_let.expression = node_expression.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            return StatementNode{.var = statment_let};
        }

        if (peek().has_value() && peek().value().type == TokenType::semicolon) {
            eat();
        } else {
            std::cerr << "Expected Semi colon" << std::endl;
            exit(EXIT_FAILURE);
        }

        return {};
    }

    std::optional<ExitNode> parse() {
        std::optional<ExitNode> exit_node;
        while (peek().has_value()) {
            if (peek().value().type == TokenType::exit && peek(1).has_value() &&
                peek(1).value().type == TokenType::openParanthesis) {
                eat();
                eat();
                if (auto node_expression = parseExpression()) {
                    exit_node = ExitNode{.expr = node_expression.value()};
                } else {
                    std::cerr << "Invalid expression pass" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            if (peek().has_value() &&
                peek().value().type == TokenType::closeParanthesis) {
                eat();
            } else {
                std::cerr << "Expected Closing Paranthesis )" << std::endl;
            }

            if (peek().has_value() &&
                peek().value().type == TokenType::semicolon) {
                eat();
            } else {
                std::cerr << "Expected Semicolon ;" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return exit_node;
    }

  private:
    const std::vector<Token> m_tokens;

    [[nodiscard]] inline std::optional<Token> peek(const int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        } else {
            return m_tokens.at(m_index + offset);
        }
    }

    Token eat() { return m_tokens.at(m_index++); }

    size_t m_index = 0;
};
