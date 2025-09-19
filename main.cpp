#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "lexer.h"
#include "parser.h"

int main() {
    defineRustTokenPatterns();

    std::string filePath = "../samples/temp.rs";
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    std::string rustCode((std::istreambuf_iterator<char>(inputFile)),
                         std::istreambuf_iterator<char>());
    inputFile.close();

    std::vector<Token> tokens = tokenizeRustCode(rustCode);

    std::cout << "--- Tokens from " << filePath << " ---" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "Type: " << tokenTypeToString(token.type)
                  << ", Value: \"" << token.value
                  << "\", Pos: " << token.number << std::endl;

        if (token.type == TokenType::Unknown) {
            std::cout << "Compile Error";
            return 0;
        }
    }

    auto parser = Parser(tokens);
    try {
        parser.work();
    }
    catch (...) {
        std::cout << "Compile Error";
        return 0;
    }

    return 0;
}
