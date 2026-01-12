#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "lexer.h"
#include "parser/parser.h"
#include "semantic/semantic_analyzer.h"

//if7 if10 array7 still with problems
//about node type

int main() {
    std::vector<std::string> testFiles = {
        "testcases/testcases/array1.in",
        "testcases/testcases/array2.in",
        "testcases/testcases/array3.in",
        "testcases/testcases/array4.in",
        "testcases/testcases/array5.in",
        "testcases/testcases/array6.in",
        "testcases/testcases/array7.in",
        "testcases/testcases/array8.in",

        "testcases/testcases/if1.in",
        "testcases/testcases/if2.in",
        "testcases/testcases/if3.in",
        "testcases/testcases/if4.in",
        "testcases/testcases/if5.in",
        "testcases/testcases/if6.in",
        "testcases/testcases/if7.in",
        "testcases/testcases/if8.in",
        "testcases/testcases/if9.in",
        "testcases/testcases/if10.in",
        "testcases/testcases/if11.in",
        "testcases/testcases/if12.in",
        "testcases/testcases/if13.in",
        "testcases/testcases/if14.in",
        "testcases/testcases/if15.in",

        "testcases/testcases/loop9.in",
    };
    
    for (const auto& filePath : testFiles) {
        std::cout << "\n=== Testing " << filePath << " ===" << std::endl;
        
        std::ifstream inputFile("../" + filePath);
        if (!inputFile.is_open()) {
            std::cerr << "Error: Could not open file " << filePath << std::endl;
            continue;
        }

        std::string rustCode((std::istreambuf_iterator<char>(inputFile)),
                             std::istreambuf_iterator<char>());
        inputFile.close();

        try {
            // 词法分析
            Lexer lexer(rustCode);
            std::vector<Token> tokens = lexer.tokenizeRustCode();

            bool hasLexError = false;
            for (const auto& token : tokens) {
                if (token.type == TokenType::Unknown) {
                    std::cout << "Lexical Error in " << filePath << std::endl;
                    hasLexError = true;
                    break;
                }
            }
            
            if (hasLexError) continue;

            // 语法分析
            auto parser = Parser(rustCode);
            AstNode* ast_root = parser.work();
            
            // 语义分析
            SemanticAnalyzer semantic_analyzer(ast_root);
            semantic_analyzer.analyze();
            
            if (semantic_analyzer.has_errors()) {
                std::cout << "Semantic Errors in " << filePath << ":" << std::endl;
                for (const auto& error : semantic_analyzer.get_errors()) {
                    std::cout << "  " << error << std::endl;
                }
            } else {
                std::cout << "Success: No semantic errors in " << filePath << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Compile Error in " << filePath << ": " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown Compile Error in " << filePath << std::endl;
        }
    }

    return 0;
}
