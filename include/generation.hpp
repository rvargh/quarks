#pragma once

class Generator {

public:
    inline explicit Generator(ExitNode root) : m_root(std::move(root)) {}

    [[nodiscard]] std::string generate() const {

        std::stringstream output;

        output << "global _start\n_start:\n";
        output << "    mov rax, 60\n";
        output << "    mov rdi, " << m_root.expr.int_literals.value.value() << "\n";
        output << "    syscall";

        return output.str();

    }
private:

    const ExitNode m_root;
};