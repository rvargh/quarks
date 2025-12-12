//
// Created by royva on 27-10-2025.
//
#pragma once
#include "tokenization.hpp"

struct ExpressionNode;

struct TermIntLiteralNode {
    Token int_literals;
};

struct TermIdentifierNode {
    Token identifier;
};

struct TermParenthesisNode {
    ExpressionNode* expression;
};

struct BinaryExpressionAddition {
    ExpressionNode* lhs;
    ExpressionNode* rhs;
};

struct BinaryExpressionSubtraction {
    ExpressionNode* lhs;
    ExpressionNode* rhs;
};

struct BinaryExpressionMultiplication {
    ExpressionNode* lhs;
    ExpressionNode* rhs;
};

struct BinaryExpressionDivision {
    ExpressionNode* lhs;
    ExpressionNode* rhs;
};

struct BinaryExpressionNode {
    std::variant<BinaryExpressionAddition*,BinaryExpressionMultiplication*,BinaryExpressionDivision*,BinaryExpressionSubtraction*> ops;
};

struct TermNode {
    std::variant<TermIdentifierNode*, TermIntLiteralNode*, TermParenthesisNode*> vars;
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

struct StatementNode;

struct ScopeStatementNode {
    std::vector<StatementNode*> statements;
};

struct StatementNode {
    std::variant<StatementExitNode*, LetStatementNode*, ScopeStatementNode*> var;
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

        if (peek().has_value() && peek().value().type == TokenType::intLiteral) {
            auto* int_literal = m_allocator.alloc<TermIntLiteralNode>();
            int_literal->int_literals = eat();
            auto* node = m_allocator.alloc<TermNode>();
            node->vars = int_literal;
            return node;
        } else if (peek().has_value() && peek().value().type == TokenType::identifier) {
            auto* identifier = m_allocator.alloc<TermIdentifierNode>();
            identifier->identifier = eat();
            auto* node = m_allocator.alloc<TermNode>();
            node->vars = identifier;
            return node;
        } else if (peek().has_value() && peek().value().type == TokenType::openParentheses) {
            eat();
            std::optional<ExpressionNode*> expr = parseExpression();
            if (!expr.has_value()) {
                std::cerr << "Expected expression\n";
                exit(EXIT_FAILURE);
            }
            if (!peek().has_value() && peek().value().type == TokenType::closeParentheses) {
                std::cerr << "Expected close parenthesis\n";
                exit(EXIT_FAILURE);
            }
            eat();
            auto* nodeTermParenthesis = m_allocator.alloc<TermParenthesisNode>();
            nodeTermParenthesis->expression = expr.value();
            auto* node = m_allocator.alloc<TermNode>();
            node->vars = nodeTermParenthesis;
            return node;
        } else {
            return std::nullopt;
        }
    }

    std::optional<ExpressionNode*> parseExpression(int minPrecedence = 0) {

        std::optional<TermNode*> termLhs = parseTerm();
        if (!termLhs.has_value()) {
            return std::nullopt;
        }
        auto* expressionLhs = m_allocator.alloc<ExpressionNode>();
        expressionLhs->var = termLhs.value();

        while (true) {
            std::optional<Token> currentToken = peek();
            std::optional<int> precedence;
            if (currentToken.has_value()) {
                precedence = isBinaryOperator(currentToken.value().type);
                if (!precedence.has_value() || precedence < minPrecedence) {
                    break;
                }
            } else {
                break;
            }
            Token ops = eat();
            int nminPrecedence = precedence.value() + 1;
            auto expressionRhs = parseExpression(nminPrecedence);
            if (!expressionRhs.has_value()) {
                std::cerr << "Unable to parse expression.\n";
                exit(EXIT_FAILURE);
            }

            auto* expression = m_allocator.alloc<BinaryExpressionNode>();
            auto* expressionLhs2 = m_allocator.alloc<ExpressionNode>();
            if (ops.type == TokenType::addition) {
                auto* add = m_allocator.alloc<BinaryExpressionAddition>();
                expressionLhs2->var = expressionLhs->var;
                add->lhs = expressionLhs2;
                add->rhs = expressionRhs.value();
                expression->ops = add;
            } else if (ops.type == TokenType::multiplication) {
                auto* mul = m_allocator.alloc<BinaryExpressionMultiplication>();
                expressionLhs2->var = expressionLhs->var;
                mul->lhs = expressionLhs2;
                mul->rhs = expressionRhs.value();
                expression->ops = mul;
            } else if (ops.type == TokenType::division) {
                auto* div = m_allocator.alloc<BinaryExpressionDivision>();
                expressionLhs2->var = expressionLhs->var;
                div->lhs = expressionLhs2;
                div->rhs = expressionRhs.value();
                expression->ops = div;
            } else if (ops.type == TokenType::substraction) {
                auto* sub = m_allocator.alloc<BinaryExpressionSubtraction>();
                expressionLhs2->var = expressionLhs->var;
                sub->lhs = expressionLhs2;
                sub->rhs = expressionRhs.value();
                expression->ops = sub;
            } else {
                assert(false); // unreachable
            }
            expressionLhs->var = expression;
        }
        return expressionLhs;
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
