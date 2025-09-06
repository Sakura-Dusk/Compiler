//
// Created by Sakura on 25-8-27.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <utility>

#include "basic.h"
#include "Literal_Type.h"
#include <optional>

class Expression: BasicNode {
public:
    virtual ~Expression() = default;

    virtual void accept(SemanticAnalyzer &visitor) = 0;
};

class LiteralExpression final : Expression {
public:
    LiteralType type;
    std::string value;

    LiteralExpression(LiteralType type, std::string value): type(std::move(type)), value(std::move(value)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class PathExpression final : Expression {
public:
    struct Expression_Args {
        std::vector<std::shared_ptr<Expression>> arguments;
    };
    struct PathPattern {
        std::string identifier;
        std::optional<Expression_Args> parameter;
    };
    bool have_leading_colon;
    std::vector<PathPattern> paths;

    PathExpression(bool have_colon, std::vector<PathPattern> paths): have_leading_colon(have_colon), paths(std::move(paths)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class BlockExpression final : Expression {
public:
    std::vector<std::shared_ptr<BasicNode>> statement;
    std::optional<std::shared_ptr<Expression>> no_block_expression;

    BlockExpression(std::vector<std::shared_ptr<BasicNode>> statement, std::optional<std::shared_ptr<Expression>> no_block_expression = std::nullopt): statement(std::move(statement)), no_block_expression(std::move(no_block_expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

#endif //EXPRESSION_H
