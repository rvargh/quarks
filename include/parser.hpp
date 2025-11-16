//
// Created by royva on 27-10-2025.
//
#pragma once

struct ExpressionNode;

struct TermIntLiteralNode {
    Token int_literals;
};

struct TermIdentifierNode {
    Token identifier;
};

struct BinaryExpressionAddition {
    ExpressionNode* lhs;
    ExpressionNode* rhs;
};

struct BinaryExpressionMultiplication {
    ExpressionNode* lhs;
    ExpressionNode* rhs;
};

struct BinaryExpressionNode {
    BinaryExpressionAddition* add;
};

// struct BinaryExpressionNode {
//     std::variant<BinaryExpressionAddition*,BinaryExpressionMultiplication*>
//     vars;
// };

struct TermNode {
    std::variant<TermIdentifierNode*, TermIntLiteralNode*> vars;
};

// these are just tokens(integer literals)
struct ExpressionNode {
    std::variant<TermNode*, BinaryExpressionNode*> var;
};

struct StatementExitNode {
    ExpressionNode* expr;
};

struct LetStatementNode {
    Token identifier;
    ExpressionNode* expression;
};

struct StatementNode {
    std::variant<StatementExitNode*, LetStatementNode*> var;
};

struct ProgramNode {
    std::vector<StatementNode*> statements;
};

struct intLiteral {};

class Parser {
  public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 4) {}

    std::optional<TermNode*> parseTerm() {

        if (peek().has_value() &&
            peek().value().type == TokenType::intLiteral) {
            auto* int_literal = m_allocator.alloc<TermIntLiteralNode>();
            int_literal->int_literals = eat();
            auto* node = m_allocator.alloc<TermNode>();
            node->vars = int_literal;
            return node;
        } else if (peek().has_value() &&
                   peek().value().type == TokenType::identifier) {
            auto* identifier = m_allocator.alloc<TermIdentifierNode>();
            identifier->identifier = eat();
            auto* node = m_allocator.alloc<TermNode>();
            node->vars = identifier;
            return node;
        } else {
            return std::nullopt;
        }
    }

    std::optional<ExpressionNode*> parseExpression() {

        if (std::optional<TermNode*> term = parseTerm()) {
            if (peek().has_value() &&
                peek().value().type == TokenType::addition) {
                auto* binary_expression_node =
                    m_allocator.alloc<BinaryExpressionNode>();
                auto* add = m_allocator.alloc<BinaryExpressionAddition>();
                auto* lhs = m_allocator.alloc<ExpressionNode>();
                lhs->var = term.value();
                add->lhs = lhs;
                eat();
                if (std::optional<ExpressionNode*> rhs = parseExpression()) {
                    add->rhs = rhs.value();
                    binary_expression_node->add = add;
                    auto* node = m_allocator.alloc<ExpressionNode>();
                    node->var = binary_expression_node;
                    return node;
                } else {
                    std::cerr << "Expected expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                auto* expression = m_allocator.alloc<ExpressionNode>();
                expression->var = term.value();
                return expression;
            }
        } else {
            return std::nullopt;
        }
    }

    std::optional<StatementNode*> parseStatement() {

        if (peek().value().type == TokenType::exit && peek(1).has_value() &&
            peek(1).value().type == TokenType::openParentheses) {
            eat();
            eat();

            auto* exitNode = m_allocator.alloc<StatementExitNode>();

            if (std::optional<ExpressionNode*> node_expression =
                    parseExpression()) {
                exitNode->expr = node_expression.value();
            } else {
                std::cerr << "Invalid expression pass" << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() &&
                peek().value().type == TokenType::closeParentheses) {
                eat();
            } else {
                std::cerr << "Expected `)`" << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() &&
                peek().value().type == TokenType::semicolon) {
                eat();
            } else {
                std::cerr << "Expected `;`" << std::endl;
                exit(EXIT_FAILURE);
            }
            auto* statement = m_allocator.alloc<StatementNode>();
            statement->var = exitNode;
            return statement;

        } else if (peek().has_value() &&
                   peek().value().type == TokenType::assign &&
                   peek(1).has_value() &&
                   peek(1).value().type == TokenType::identifier &&
                   peek(2).has_value() &&
                   peek(2).value().type == TokenType::equals) {
            // assign(variable declaration) since we don't need it.
            eat();
            // identifier we eat
            auto* statement_let = m_allocator.alloc<LetStatementNode>();
            statement_let->identifier = eat();
            eat();
            if (std::optional<ExpressionNode*> node_expression =
                    parseExpression()) {
                statement_let->expression = node_expression.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() &&
                peek().value().type == TokenType::semicolon) {
                eat();
            } else {
                std::cerr << "Expected ;" << std::endl;
                exit(EXIT_FAILURE);
            }

            auto* statement = m_allocator.alloc<StatementNode>();
            statement->var = statement_let;
            return statement;
        } else {
            return {};
        }
    }

    std::optional<ProgramNode> parseProgram() {
        ProgramNode program;
        while (peek().has_value()) {
            if (std::optional<StatementNode*> stmt = parseStatement()) {
                program.statements.push_back(stmt.value());
            } else {
                std::cerr << "Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return program;
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

    ArenaAllocator m_allocator;
};
