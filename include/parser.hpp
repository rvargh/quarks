//
// Created by roy varghese on 27-10-2025.
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
    ExpressionNode* expression {};
};

struct StatementNode;

struct nodeScope {
    std::vector<StatementNode*> statements;
};

struct nodeIfPredicate;

struct nodeIfPredicateElif {
    ExpressionNode* expression {};
    nodeScope* scope {};
    std::optional<nodeIfPredicate*> ifPredicate;
};

struct nodeIfPredicateElse {
    nodeScope* scope;
};

struct nodeIfPredicate {
    std::variant<nodeIfPredicateElif*,nodeIfPredicateElse*> predicate;
};

struct nodeIfStatement {
    ExpressionNode* expression {};
    nodeScope* scope{};
    std::optional<nodeIfPredicate*> ifPredicate;
};

struct nodeStatementAssign {
    Token identifier;
    ExpressionNode* expression {};
};

struct StatementNode {
    std::variant<StatementExitNode*, LetStatementNode*,nodeScope* ,nodeIfStatement*,nodeStatementAssign*> var;
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

    std::optional<ExpressionNode*> parseExpression(const int minPrecedence = 0) {

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
            int currentPrecedence = precedence.value() + 1;
            auto expressionRhs = parseExpression(currentPrecedence);
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

    std::optional<nodeScope*> parse_scope() {

        if (!try_consume(TokenType::open_curly).has_value()) {
            return std::nullopt;
        }
        auto scope = m_allocator.alloc<nodeScope>();
        while (auto stmt = parseStatement()) {
            scope->statements.push_back(stmt.value());
        }

        try_consume(TokenType::close_curly,"Expected `}`");
        return scope;
    }

    std::optional<nodeIfPredicate*> parse_if_predicate()
    {
        if (try_consume(TokenType::elif)) {
            try_consume(TokenType::openParentheses,"Expected `(`");
            const auto elif = m_allocator.alloc<nodeIfPredicateElif>();
            if (const auto expression = parseExpression()) {
                elif->expression = expression.value();
            } else {
                std::cerr << "Expected Expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::closeParentheses,"Expected `)`");
            if (const auto scope = parse_scope()) {
                elif->scope = scope.value();
            } else {
                std::cerr << "Expected Scope" << std::endl;
                exit(EXIT_FAILURE);
            }

            elif->ifPredicate = parse_if_predicate();
            auto predicate = m_allocator.emplace<nodeIfPredicate>(elif);
            return predicate;
        }

        if (try_consume(TokenType::else_)) {
            auto else_ = m_allocator.alloc<nodeIfPredicateElse>();
            if (const auto scope = parse_scope()) {
                else_->scope = scope.value();
            } else {
                std::cerr << "Expected scope" << std::endl;
                exit(EXIT_FAILURE);
            }
            auto predicate = m_allocator.emplace<nodeIfPredicate>(else_);
            return predicate;
        }
        return std::nullopt;
    }

    std::optional<StatementNode*> parseStatement() {

        if (peek().value().type == TokenType::exit && peek(1).has_value() &&
            peek(1).value().type == TokenType::openParentheses) {
            eat();
            eat();

            auto* exitNode = m_allocator.alloc<StatementExitNode>();
            if (const std::optional<ExpressionNode*> node_expression =
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
        }

        if (peek().has_value() &&
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
        }

        if (peek().has_value() && peek().value().type == TokenType::identifier
                && peek(1).has_value() && peek(1).value().type == TokenType::equals) {
            auto* assign = m_allocator.alloc<nodeStatementAssign>();
            assign->identifier = eat();
            eat();
            if (const auto expression = parseExpression()) {
                assign->expression = expression.value();
            } else {
                std::cerr << "Expected Expression" <<std::endl;
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::semicolon,"Expected semicolon");
            auto stmt = m_allocator.emplace<StatementNode>(assign);
            return stmt;
        }

        if (peek().has_value() && peek().value().type == TokenType::open_curly) {
            if (auto scope = parse_scope()) {
                auto stmt = m_allocator.alloc<StatementNode>();
                stmt->var = scope.value();
                return stmt;
            } else {
                std::cerr << "Invalid scope" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        if (auto if_ = try_consume(TokenType::if_)) {
            try_consume(TokenType::openParentheses, "Expected `(`");
            auto statement_if = m_allocator.alloc<nodeIfStatement>();
            if (auto expr = parseExpression()) {
                statement_if->expression = expr.value();
            } else {
                std::cerr << "Invalid Expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::closeParentheses, "Expected `)`");
            if (const auto scope = parse_scope()) {
                statement_if->scope = scope.value();
            } else {
                std::cerr << "Invalid scope" << std::endl;
                exit(EXIT_FAILURE);
            }
            statement_if->ifPredicate = parse_if_predicate();
            auto statement = m_allocator.alloc<StatementNode>();
            statement->var = statement_if;
            return statement;
        }

        return {};
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

    inline Token try_consume(TokenType type, const std::string& err_msg)
    {
        if (peek().has_value() && peek().value().type == type) {
            return eat();
        }
        else {
            std::cerr << err_msg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    inline std::optional<Token> try_consume(TokenType type)
    {
        if (peek().has_value() && peek().value().type == type) {
            return eat();
        }
        else {
            return {};
        }
    }

    Token eat() { return m_tokens.at(m_index++); }

    size_t m_index = 0;

    ArenaAllocator m_allocator;
};
