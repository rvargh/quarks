#pragma once
#include <unordered_map>

class Generator {

  public:
    inline explicit Generator(ProgramNode program)
        : m_program(std::move(program)) {}

    void generateExpression(const ExpressionNode* expression) {

        struct ExpressionNodeVisitor {

            Generator* m_generator;

            void operator()(const ExpressionIntLiteralNode* int_literal) const {
                m_generator->m_output << "    mov rax, "
                                      << int_literal->int_literals.value.value()
                                      << "\n";
                m_generator->push("rax");
            }

            void operator()(const ExpressionIdentifierNode* identifier_expression) const {

                if (!m_generator->m_vars.contains(
                        identifier_expression->identifier.value.value())) {
                    std::cerr << "Undeclared identifier: "
                              << identifier_expression->identifier.value.value()
                              << std::endl;
                    exit(EXIT_FAILURE);
                }

                const Variables& var = m_generator->m_vars.at(
                    identifier_expression->identifier.value.value());

                std::stringstream offset;

                offset << "QWORD [rsp + "
                       << (m_generator->m_stack_size - var.stack_location - 1) * 8
                       << "]";
                m_generator->push(offset.str());
            }

            void operator()(const BinaryExpressionNode* binaryNode) const {
                assert(false);
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

                if (m_generator->m_vars.contains(
                        stmt_let->identifier.value.value())) {
                    std::cerr << "Identifier already used: "
                              << stmt_let->identifier.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                m_generator->m_vars.insert({
                        stmt_let->identifier.value.value(),
                        Variables{.stack_location = m_generator->m_stack_size}
                    });
                m_generator->generateExpression(stmt_let->expression);
                // m_generator->push("rax");
            }
            void operator()(const StatementExitNode* stmt_exit) const {

                m_generator->generateExpression(stmt_exit->expr);

                m_generator->m_output << "    mov rax, 60\n";
                m_generator->pop("rdi");
                m_generator->m_output << "    syscall\n";
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
        size_t stack_location;
    };

    std::unordered_map<std::string, Variables> m_vars;

    void push(const std::string& reg) {
        m_output << "    push " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg) {
        m_output << "    pop " << reg << "\n";
        m_stack_size--;
    }
};