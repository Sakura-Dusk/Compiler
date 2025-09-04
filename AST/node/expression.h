//
// Created by Sakura on 25-8-27.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <utility>

#include "basic.h"
#include "Literal_Type.h"

class Expression: BasicNode {
public:
    virtual void accept(SemanticAnalyzer &visitor) = 0;
};

class LiteralExpression: Expression {
public:
    LiteralType type;
    std::string value;

    LiteralExpression(LiteralType type, std::string value): type(std::move(type)), value(std::move(value)) {}

    void accept(SemanticAnalyzer &visitor) override {visitor.visit(*this);}
};

class PathExpression: Expression {
public:

};

#endif //EXPRESSION_H
