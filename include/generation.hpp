#pragma once

class Generator {

  public:
    inline explicit Generator(ProgramNode program)
        : m_program(std::move(program)) {}


    void generateExpression(const ExpressionNode& expression) {

        struct ExpressionNodeVisitor {

            Generator* m_generator;

            void operator()(const ExpressionIntLiteralNode& int_literal) const {

                m_generator->m_output << "    mov rax, " << int_literal.int_literals.value.value() << "\n";
                m_generator->m_output << "    push rax\n";

            }
            void operator()(const ExpressionIdentifierNode& identifier_expression) const {  }

        };

        ExpressionNodeVisitor visitor{.m_generator = this};
        std::visit(visitor,expression.var);

    }

    [[nodiscard]] std::string generateProgram() {

        m_output << "global _start\n_start:\n";

        for (const StatementNode& statement : m_program.statements) {
            generateStatement(statement);
        }
        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 0\n";
        m_output << "    syscall\n";

        return m_output.str();
    }

    void generateStatement(const StatementNode& stmt) {

        struct StatementVisitor {
            Generator* m_generator;

            void operator()(const LetStatementNode& stmt_let) const {

            }
            void operator()(const StatementExitNode& stmt_exit) const {

                m_generator->generateExpression(stmt_exit.expr);

                m_generator->m_output << "    mov rax, 60\n";
                m_generator->m_output << "    pop rdi\n";
                m_generator->m_output << "    syscall\n";
            }
        };

        StatementVisitor visitor{.m_generator = this};
        std::visit(visitor, stmt.var);
    }

  private:
    const ProgramNode m_program;
    std::stringstream m_output;
};