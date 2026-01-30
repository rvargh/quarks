#pragma once
#include "parser.hpp"
#include <algorithm>

class Generator {

  public:
    inline explicit Generator(ProgramNode program)
        : m_program(std::move(program)) {}

    void generateTerm(const TermNode* expression) {
        struct TermVisitor {

            Generator& m_generator;

            void operator()(const TermIdentifierNode* id) const {

                const auto it = ranges::find_if(
                    m_generator.m_vars,
                    [&](const Variables& variables) {
                           return variables.name == id->identifier.value.value();
                       });

                if (it  == m_generator.m_vars.end())
                {
                    std::cerr << "Undeclared identifier: "
                              << id->identifier.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                std::stringstream offset;

                offset << "QWORD [rsp + "
                       << (m_generator.m_stack_size - it->stack_location - 1) *
                              8
                       << "]";
                m_generator.push(offset.str());
            }

            void operator()(const TermIntLiteralNode* it) const {

                m_generator.m_output << "    mov rax, "
                                    << it->int_literals.value.value() << "\n";
                m_generator.push("rax");
            }

            void operator()(const TermParenthesisNode* pn) const {
                m_generator.generateExpression(pn->expression);
            }
        };

        TermVisitor visitor{.m_generator = *this};
        std::visit(visitor, expression->vars);
    }

    void generateBinaryExpression(const BinaryExpressionNode* bns) {

        struct binaryExpressionVisitor {

            Generator& m_generator;

            void operator()(const BinaryExpressionAddition* add) const {
                m_generator.generateExpression(add->rhs);
                m_generator.generateExpression(add->lhs);
                m_generator.pop("rax");
                m_generator.pop("rbx");
                m_generator.m_output << "    ADD rax, rbx\n";
                m_generator.push("rax");
            }

            void operator()(const BinaryExpressionSubtraction* sub) const {
                m_generator.generateExpression(sub->rhs);
                m_generator.generateExpression(sub->lhs);
                m_generator.pop("rax");
                m_generator.pop("rbx");
                m_generator.m_output << "    SUB rax, rbx\n";
                m_generator.push("rax");
            }

            void operator()(const BinaryExpressionMultiplication* mul) const {
                m_generator.generateExpression(mul->rhs);
                m_generator.generateExpression(mul->lhs);
                m_generator.pop("rax");
                m_generator.pop("rbx");
                m_generator.m_output << "    MUL rbx\n";
                m_generator.push("rax");
            }

            void operator()(const BinaryExpressionDivision* div) const {
                m_generator.generateExpression(div->rhs);
                m_generator.generateExpression(div->lhs);
                m_generator.pop("rax");
                m_generator.pop("rbx");
                m_generator.m_output << "    DIV rbx\n";
                m_generator.push("rax");
            }
        };

        binaryExpressionVisitor visitor{.m_generator = *this};
        std::visit(visitor,bns->ops);
    }

    void generateExpression(const ExpressionNode* expression) {

        struct ExpressionNodeVisitor {

            Generator& m_generator;

            void operator()(const TermNode* term) const {
                m_generator.generateTerm(term);
            }

            void operator()(const BinaryExpressionNode* binaryNode) const {
                m_generator.generateBinaryExpression(binaryNode);
            }
        };

        ExpressionNodeVisitor visitor{.m_generator = *this};
        std::visit(visitor, expression->var);
    }

    [[nodiscard]] std::string generateProgram() {

        m_output << "global _start\n_start:\n";

        for (const StatementNode* statement : m_program.statements) {
            generateStatement(statement);
        }
        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 0\n";
        m_output << "    syscall\n";

        return m_output.str();
    }

    void generate_scope(const nodeScope* scope)
    {
        begin_scope();
        for (const StatementNode* stmt : scope->statements) {
            generateStatement(stmt);
        }
        end_scope();
    }

    void generate_if_predicate(const nodeIfPredicate* predicate,const std::string& end_label) {

        struct predicateVisitor {
            Generator& m_generator;
            const std::string& end_label;

            void operator()(const nodeIfPredicateElif* elif) const
            {
                m_generator.generateExpression(elif->expression);
                m_generator.pop("rax");
                const std::string label = m_generator.create_label();
                m_generator.m_output << "    test rax, rax\n";
                m_generator.m_output << "    jz " << label <<"\n";
                m_generator.generate_scope(elif->scope);
                m_generator.m_output << "    jmp " << end_label << "\n";
                if (elif->ifPredicate.has_value()) {
                    m_generator.m_output << label << ":\n";
                    m_generator.generate_if_predicate(elif->ifPredicate.value(), end_label);
                }
            }

            void operator()(const nodeIfPredicateElse* _else) const
            {
                m_generator.generate_scope(_else->scope);
            }
        };

        predicateVisitor visitor{.m_generator = *this, .end_label = end_label};
        std::visit(visitor, predicate->predicate);
    }

    void generateStatement(const StatementNode* stmt) {

        struct StatementVisitor {
            Generator& m_generator;

            void operator()(const LetStatementNode* stmt_let) const {

                const auto it = ranges::find_if(
                    m_generator.m_vars,
                    [&](const Variables& variables) {
                       return variables.name == stmt_let->identifier.value.value();
                   });
                if (it != m_generator.m_vars.cend()) {
                    std::cerr << "Identifier already used: "
                              << stmt_let->identifier.value.value()
                              << std::endl;
                    exit(EXIT_FAILURE);
                }
                m_generator.m_vars.push_back({
                    Variables{.name = stmt_let->identifier.value.value(),.stack_location = m_generator.m_stack_size}
                });
                m_generator.generateExpression(stmt_let->expression);
            }
            void operator()(const StatementExitNode* stmt_exit) const {
                m_generator.generateExpression(stmt_exit->expr);
                m_generator.m_output << "    mov rax, 60\n";
                m_generator.pop("rdi");
                m_generator.m_output << "    syscall\n";
            }

            void operator()(const nodeScope* scope) const
            {
                m_generator.generate_scope(scope);
            }

            void operator()(const nodeIfStatement* statement_if) const {
                m_generator.generateExpression(statement_if->expression);
                m_generator.pop("rax");
                const std::string label = m_generator.create_label();
                m_generator.m_output << "    test rax, rax\n";
                m_generator.m_output << "    jz " << label <<"\n";
                m_generator.generate_scope(statement_if->scope);
                m_generator.m_output << label << ":\n";
                if (statement_if->ifPredicate.has_value()) {
                    const std::string end_label = m_generator.create_label();
                    m_generator.generate_if_predicate(statement_if->ifPredicate.value(), end_label);
                    m_generator.m_output << end_label << ":\n";
                }
            }
        };
        StatementVisitor visitor{.m_generator = *this};
        std::visit(visitor, stmt->var);
    }

  private:
    const ProgramNode m_program;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    int m_label_count = 0;

    struct Variables {
        std::string name;
        size_t stack_location;
    };

    std::vector<Variables> m_vars {};
    std::vector<size_t> m_scopes {};

    void push(const std::string& reg) {
        m_output << "    push " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg) {
        m_output << "    pop " << reg << "\n";
        m_stack_size--;
    }

    void begin_scope() {
        m_scopes.push_back(m_vars.size());
    }

    void end_scope() {
        const size_t pop_count = m_vars.size() - m_scopes.back();
        m_output << "    add rsp, " << pop_count * 8 << "\n";
        m_stack_size -= pop_count;
        for (int i = 0; i < pop_count; i++) {
            m_vars.pop_back();
        }
        m_scopes.pop_back();
    }

    std::string create_label() {
        std::stringstream ss;
        ss << "label" << m_label_count++;
        return ss.str();
    }
};