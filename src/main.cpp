#include "../include/tokenization.hpp"

std::string tokenToASM(const std::vector<Token>& tokens) {
    std::stringstream output;

    output << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const auto& [tokenType,tokenValue] = tokens.at(i);
        if (tokenType == TokenType::exit) {
           if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_literal) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semicolon) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens.at(i + 1).value.value()
                           << "\n";
                    output << "    syscall";
               }
            }
        }
    }

    return output.str();
}

int main(int argc, char* argv[]) {

    // if (argc != 2) {
    //     std::cerr << "Incorrect usage. Correct usage is ..\n";
    //     std::cerr << "quarks <*.hy>\n";
    //     return EXIT_FAILURE;
    // }

    const char* filepath = "/home/darkknight/cpp/quarks/sample/test.qs";

    std::string content;
    /** inner scope to close the file once read is completed */
    {
        std::ifstream fileIn(filepath);
        if (!fileIn) {
            std::cerr << "Failed to open kkkkfile \n";
            return EXIT_FAILURE;
        }

        std::stringstream buffer;
        buffer << fileIn.rdbuf();
        content = buffer.str();
    }
    Tokenizer tokenizer(std::move(content));

    const std::vector<Token> things = tokenizer.tokenize();

    {
        std::ofstream file("../out.asm");
        std::cout << "tokens not working" << tokenToASM(things) << std::endl;
        file << tokenToASM(things);
    }
    std::cout << "I am checking things : "<< things.data() << std::endl;

    
    std::system("nasm -felf64 ../out.asm");
    std::system("ld -o ../out ../out.o");

    std::cout << argc << std::endl;
    std::cout << argv[0] << std::endl;
    std::cout << argv[1] << std::endl;

    return EXIT_SUCCESS;
}