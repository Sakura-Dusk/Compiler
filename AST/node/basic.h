//
// Created by Sakura on 25-8-27.
//

#ifndef BASIC_H
#define BASIC_H
#include "expression.h"

class SemanticAnalyzer;

class BasicNode {
public:
    virtual void accept(SemanticAnalyzer &visitor) = 0;
};

class SemanticAnalyzer {
public:
    virtual void visit(BasicNode &node) = 0;

    void visit(const LiteralExpression & node);
};

#endif //BASIC_H
