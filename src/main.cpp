#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "lexer.h"
#include "parser/parser.h"
#include "semantic/semantic.h"

//if7 if10 array7 still with problems
//about node type

int main() {
    std::vector<std::string> testFiles = {
        // "testcases/testcases/test.in",
        "testcases/testcases/misc14.in",
    };
    
    for (const auto& filePath : testFiles) {
        std::cout << "\n=== Testing " << filePath << " ===" << std::endl;
        
        std::ifstream inputFile("" + filePath);
        if (!inputFile.is_open()) {
            std::cerr << "Error: Could not open file " << filePath << std::endl;
            continue;
        }

        std::string rustCode((std::istreambuf_iterator<char>(inputFile)),
                             std::istreambuf_iterator<char>());
        inputFile.close();

        try {
            // 语法分析
            auto parser = Parser(rustCode);
            AstNode* ast_root = parser.work();

            auto opt = ast_root->show_tree();
            for (auto str: opt) std::cout << str << std::endl;
            
            // 语义分析
            Semantic_check(ast_root);
        } catch (const std::exception& e) {
            std::cout << "Compile Error in " << filePath << ": " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown Compile Error in " << filePath << std::endl;
        }
    }

    return 0;
}
