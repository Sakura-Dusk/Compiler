//
// Created by Sakura on 25-9-17.
//

#ifndef PARSER_H
#define PARSER_H
#include <utility>

#include "../AST/node/basic.h"
#include "../lexer.h"
#include "vector"

class ProgramNode: public BasicNode {
public:
    std::vector<std::shared_ptr<BasicNode>> statements;

    ProgramNode() = default;
    explicit ProgramNode(std::vector<std::shared_ptr<BasicNode>> statements) : statements(move(statements)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class Parser {
    std::vector<Token> tokens;
    int token_pos;
    void parser_program(std::shared_ptr<ProgramNode>);

public:
    Parser();
    explicit Parser(std::vector<Token>);

    std::shared_ptr<BasicNode> work() {
        auto program = std::make_shared<ProgramNode>();
        while (token_pos < tokens.size()) {
            auto now_token = tokens[token_pos];
            if (now_token.type == TokenType::Keyword && now_token.value == "fn") {
                parser_program(program);
            }
        }

        return program;
    }
};

#endif //PARSER_H
