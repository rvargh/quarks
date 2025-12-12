#pragma once
#include "parser.hpp"

#include <algorithm>

class Generator {

  public:
    inline explicit Generator(ProgramNode program)
        : m_program(std::move(program)) {}

    void generateTerm(const TermNode* expression) {
        struct TermVisitor {

            Generator* m_generator;

            void operator()(const TermIdentifierNode* id) const {

                const auto it = std::find_if(m_generator->m_vars.begin(),
                    m_generator->m_vars.end(),
                    [&](const Variables& variables) {
                           return variables.name == id->identifier.value.value();
                       });

                if (it  == m_generator->m_vars.end())
                {
                    std::cerr << "Undeclared identifier: "
                              << id->identifier.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                std::stringstream offset;

                offset << "QWORD [rsp + "
                       << (m_generator->m_stack_size - (*it).stack_location - 1) *
                              8
                       << "]";
                m_generator->push(offset.str());
            }

            void operator()(const TermIntLiteralNode* it) const {

                m_generator->m_output << "    mov rax, "
                                      << it->int_literals.value.value() << "\n";
                m_generator->push("rax");
            }

            void operator()(const TermParenthesisNode* pn) const {
                m_generator->generateExpression(pn->expression);
            }
        };

        TermVisitor visitor{.m_generator = this};
        std::visit(visitor, expression->vars);
    }

    void generateBinaryExpression(const BinaryExpressionNode* bns) {

        struct binaryExpressionVisitor {

            Generator* m_generator;

            void operator()(const BinaryExpressionAddition* add) const {
                m_generator->generateExpression(add->rhs);
                m_generator->generateExpression(add->lhs);
                m_generator->pop("rax");
                m_generator->pop("rbx");
                m_generator->m_output << "    ADD rax, rbx\n";
                m_generator->push("rax");
            }

            void operator()(const BinaryExpressionSubtraction* sub) const {
                m_generator->generateExpression(sub->rhs);
                m_generator->generateExpression(sub->lhs);
                m_generator->pop("rax");
                m_generator->pop("rbx");
                m_generator->m_output << "    SUB rax, rbx\n";
                m_generator->push("rax");
            }

            void operator()(const BinaryExpressionMultiplication* mul) const {
                m_generator->generateExpression(mul->rhs);
                m_generator->generateExpression(mul->lhs);
                m_generator->pop("rax");
                m_generator->pop("rbx");
                m_generator->m_output << "    MUL rbx\n";
                m_generator->push("rax");
            }

            void operator()(const BinaryExpressionDivision* div) const {
                m_generator->generateExpression(div->rhs);
                m_generator->generateExpression(div->lhs);
                m_generator->pop("rax");
                m_generator->pop("rbx");
                m_generator->m_output << "    DIV rbx\n";
                m_generator->push("rax");
            }
        };

        binaryExpressionVisitor visitor{.m_generator = this};
        std::visit(visitor,bns->ops);
    }

    void generateExpression(const ExpressionNode* expression) {

        struct ExpressionNodeVisitor {

            Generator* m_generator;

            void operator()(const TermNode* term) const {
                m_generator->generateTerm(term);
            }

            void operator()(const BinaryExpressionNode* binaryNode) const {
                m_generator->generateBinaryExpression(binaryNode);
            }
        };

        ExpressionNodeVisitor visitor{.m_generator = this};
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

    void generateStatement(const StatementNode* stmt) {

        struct StatementVisitor {
            Generator* m_generator;

            void operator()(const LetStatementNode* stmt_let) const {

                const auto it = std::find_if(m_generator->m_vars.begin(),
                    m_generator->m_vars.end(),
                    [&](const Variables& variables) {
                       return variables.name == stmt_let->identifier.value.value();
                   });
                if (it != m_generator->m_vars.cend()) {
                    std::cerr << "Identifier already used: "
                              << stmt_let->identifier.value.value()
                              << std::endl;
                    exit(EXIT_FAILURE);
                }
                m_generator->m_vars.push_back({
                    Variables{.name = stmt_let->identifier.value.value(),.stack_location = m_generator->m_stack_size}
                });
                m_generator->generateExpression(stmt_let->expression);
            }
            void operator()(const StatementExitNode* stmt_exit) const {
                m_generator->generateExpression(stmt_exit->expr);
                m_generator->m_output << "    mov rax, 60\n";
                m_generator->pop("rdi");
                m_generator->m_output << "    syscall\n";
            }

            void operator()(const ScopeStatementNode* stmt_exit) const {

            }
        };
        StatementVisitor visitor{.m_generator = this};
        std::visit(visitor, stmt->var);
    }

  private:
    const ProgramNode m_program;
    std::stringstream m_output;
    size_t m_stack_size = 0;

    struct Variables {
        std::string name;
        size_t stack_location;
    };

    std::vector<Variables> m_vars;

    void push(const std::string& reg) {
        m_output << "    push " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg) {
        m_output << "    pop " << reg << "\n";
        m_stack_size--;
    }
};