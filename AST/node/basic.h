//
// Created by Sakura on 25-8-27.
//v

#ifndef BASIC_H
#define BASIC_H
#include "expression.h"

class SemanticAnalyzer;

class BasicNode {
public:
    virtual ~BasicNode() = default;

    virtual void accept(SemanticAnalyzer &visitor) = 0;
};

class SemanticAnalyzer {
public:
    virtual void visit(BasicNode &node) = 0;

    void visit(const LiteralExpression & node);
    void visit(const PathExpression & node);
    void visit(const BlockExpression & node);
    void visit(const BorrowExpression & node);
    void visit(const DereferenceExpression & node);
    void visit(const NegateExpression & node);
    void visit(const ArithmeticOrLogicalExpression & node);
    void visit(const ComparisonExpression & node);
    void visit(const LazyBooleanExpression & node);
    void visit(const TypeCastExpression & node);
    void visit(const AssignmentExpression & node);
    void visit(const CompoundAssignmentExpression & node);
    void visit(const GroupedExpression & node);
    void visit(const IndexExpression & node);
    void visit(const StructExpression & node);
    void visit(const CallExpression & node);
    void visit(const MethodCallExpression & node);
    void visit(const FieldAccessExpression & node);
    void visit(const InfiniteExpression & node);
    void visit(const PredicateExpression & node);

};

#endif //BASIC_H
