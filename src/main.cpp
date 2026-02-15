#include "../include/common.hpp"
#include "../include/arenaAllocator.hpp"
#include "../include/tokenization.hpp"
#include "../include/parser.hpp"
#include "../include/generation.hpp"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is ..\n";
        std::cerr << "quarks <*.qs>\n";
        return EXIT_FAILURE;
    }

    std::string content;
    /** inner scope to close the file once read is completed */
    {
        const char* filepath = "/home/darkknight/cpp/quarks/sample/test.qs";
        std::ifstream fileIn(filepath);
        if (!fileIn) {
            std::cerr << "Failed to open file \n";
            return EXIT_FAILURE;
        }

        std::stringstream buffer;
        buffer << fileIn.rdbuf();
        content = buffer.str();
    }
    Tokenizer tokenizer(std::move(content));

    std::vector<Token> things = tokenizer.tokenize();
    Parser parser(std::move(things));

    std::optional<ProgramNode> program = parser.parseProgram();

    if (!program.has_value()) {
        std::cerr << "Invalid program \n";
        exit(EXIT_FAILURE);
    }

    {
        Generator generator(std::move(program.value()));
        std::ofstream file("../out.asm");
        file << generator.generateProgram();
    }

    std::system("nasm -felf64 ../out.asm");
    std::system("ld -o ../out ../out.o");

    std::cout << argc << std::endl;
    std::cout << argv[0] << std::endl;
    std::cout << argv[1] << std::endl;

    return EXIT_SUCCESS;
}