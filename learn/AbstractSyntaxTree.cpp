//
// Created by royva on 08-11-2025.
//

#include <array>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_set>
#include <map>
#include <deque>
#include <memory>
#include <variant>
#include <charconv>

enum TokenType
{
    INT_LITERALS,
    OPERATORS,
    SEMICOLON
};

struct Tokens
{
    TokenType type;
    std::string value;
};

// base class for all AST nodes
class AbstractSyntaxNode
{
public:
    virtual ~AbstractSyntaxNode() = default;

    virtual int print() const
    {
        return 0;
    }

    virtual AbstractSyntaxNode *left() const
    {
        return nullptr;
    }

    virtual AbstractSyntaxNode *right() const
    {
        return nullptr;
    }

    virtual void printClassName() const
    {
        std::cout << "__PRETTY_FUNCTION__: " << __PRETTY_FUNCTION__ << std::endl;
    }
};

// class for expressions
class ExpressionNode : public AbstractSyntaxNode
{
public:
};

class OperatorsNode : public AbstractSyntaxNode
{
public:
    OperatorsNode(char ops)
        : m_operator(std::move(ops)) {}

private:
    char m_operator;
};

// class for number literals
class NumberExpressionNode : public AbstractSyntaxNode
{
private:
    int m_val;

public:
    NumberExpressionNode(int val)
        : m_val(val) {}

    int print() const
    {
        return m_val;
    }

    void printClassName() const override
    {
        std::cout << "__PRETTY_FUNCTION__: " << __PRETTY_FUNCTION__ << std::endl;
    }
};

class BinaryOperationNode : public AbstractSyntaxNode
{
public:
    BinaryOperationNode(char ops, std::unique_ptr<AbstractSyntaxNode> left, std::unique_ptr<AbstractSyntaxNode> right)
        : m_op(ops), m_left(std::move(left)), m_right(std::move(right)) {}

    AbstractSyntaxNode *left() const override
    {
        return m_left.get();
    }

    AbstractSyntaxNode *right() const override
    {
        return m_right.get();
    }

    void printClassName() const override
    {
        std::cout << "__PRETTY_FUNCTION__: " << __PRETTY_FUNCTION__ << std::endl;
    }

private:
    char m_op;
    std::unique_ptr<AbstractSyntaxNode> m_left;
    std::unique_ptr<AbstractSyntaxNode> m_right;
};

class Tokenizer
{
public:
    explicit Tokenizer(std::string str)
        : m_string(std::move(str)) {}

    std::vector<Tokens> tokenize()
    {
        std::string buffer;
        std::vector<Tokens> tokens;

        while (peek().has_value())
        {
            if (std::isdigit(peek().value()))
            {
                buffer.push_back(eat());
                while (peek().has_value() && std::isdigit(peek().value()))
                {
                    buffer.push_back(eat());
                }
                tokens.push_back({.type = TokenType::INT_LITERALS, .value = buffer});
                buffer.clear();
                continue;
            }

            if (operators.count(peek().value()))
            {
                buffer.push_back(eat());
                tokens.push_back({.type = TokenType::OPERATORS, .value = buffer});
                buffer.clear();
                continue;
            }

            if (peek().value() == ';')
            {
                buffer.push_back(eat());
                tokens.push_back({.type = TokenType::SEMICOLON, .value = buffer});
                eat();
                continue;
            }
            eat();
        }
        return tokens;
    }

private:
    std::string m_string;
    size_t m_index = 0;
    std::array<char, 5> expressions = {'+', '-', '=', '*', '%'};

    inline std::optional<char> peek() const
    {
        if (m_index < m_string.size())
            return m_string[m_index];

        return std::nullopt;
    }

    char eat()
    {
        return m_string[m_index++];
    }

    std::unordered_set<char> operators = {'+', '-', '=', '*', '%'};
};

class Parser
{
private:
    std::vector<Tokens> m_lexers;
    size_t m_index = 0;
    std::map<char, int> BinaryOperationPrecedence{{'<', 10}, {'+', 20}, {'-', 20}, {'*', 30}};

    std::optional<Tokens> peek()
    {
        if (m_index < m_lexers.size())
            return m_lexers[m_index];

        return std::nullopt;
    }

    Tokens eat()
    {
        return m_lexers[m_index++];
    }

public:
    Parser(std::vector<Tokens> lexers)
        : m_lexers(std::move(lexers)) {}

    std::unique_ptr<AbstractSyntaxNode> parsePrimary()
    {
        std::unique_ptr<AbstractSyntaxNode> expressionNode;

        if (peek().value().type == TokenType::INT_LITERALS)
        {
            int num = std::stoi(peek().value().value);
            expressionNode = std::make_unique<NumberExpressionNode>(num);
            eat();
        }
        return expressionNode;
    }

    std::unique_ptr<AbstractSyntaxNode> parseExpression(int minPrecedence = 0)
    {
        std::unique_ptr<AbstractSyntaxNode> lhs = parsePrimary();
        std::cout << lhs->print() << std::endl;
        // lhs = 10
        // static int minPrecedence = 0;

        while (peek().has_value())
        {

            Tokens current = peek().value();

            if (current.type == TokenType::SEMICOLON)
            {
                break;
            }

            char ops = peek().value().value.front();
            auto currentPrecedence = BinaryOperationPrecedence.at(ops);

            if (currentPrecedence < minPrecedence)
            {
                break;
            }
            eat();

            std::unique_ptr<AbstractSyntaxNode> rhs = parseExpression(currentPrecedence + 1);
            lhs = std::make_unique<BinaryOperationNode>(ops, std::move(rhs), std::move(lhs));
        }
        return lhs;
    }

    std::unique_ptr<AbstractSyntaxNode> parse()
    {
        return parseExpression();
    }
};

int main()
{
    std::vector<Tokens> toks;
    std::string expression = "10 + (5 * 666)";
    Tokenizer tokenizer(expression);

    toks = tokenizer.tokenize();

    Parser parser(toks);

    std::unique_ptr<AbstractSyntaxNode> parsedTree = parser.parseExpression();
    std::cout << "i am getting out";

    std::cin.get();
}