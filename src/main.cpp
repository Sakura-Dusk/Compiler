#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "lexer.h"
#include "parser/parser.h"

int main() {
    std::string filePath = "../testcases/basic/basic5.txt";
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    std::string rustCode((std::istreambuf_iterator<char>(inputFile)),
                         std::istreambuf_iterator<char>());
    inputFile.close();

    {//Lexer show
        Lexer lexer(rustCode);
        std::vector<Token> tokens = lexer.tokenizeRustCode();

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
    }

    auto parser = Parser(rustCode);
    try {
        auto Ast_Tree = parser.work()->show_tree();
        for (const auto& x: Ast_Tree) std::cout << x << std::endl;
    }
    catch (std::exception &e) {
        std::cout << "Compile Error";
        return 0;
    }
    catch (...) {
        std::cout << "Compile Error";
        return 0;
    }

    return 0;
}
