#include "../include/common.hpp"
#include "../include/tokenization.hpp"
#include "../include/parser.hpp"
#include "../include/generation.hpp"

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

    std::vector<Token> things = tokenizer.tokenize();
    Parser parser(std::move(things));

    std::optional<ExitNode> tree = parser.parse();

    if (!tree.has_value()) {
        std::cerr << "No Exit statment found \n";
        exit(EXIT_FAILURE);
    }

    {
        Generator generator(std::move(tree.value()));
        std::ofstream file("../out.asm");
        file << generator.generate();
    }
    std::cout << "I am checking things : "<< things.data() << std::endl;

    
    std::system("nasm -felf64 ../out.asm");
    std::system("ld -o ../out ../out.o");

    std::cout << argc << std::endl;
    std::cout << argv[0] << std::endl;
    std::cout << argv[1] << std::endl;

    return EXIT_SUCCESS;
}