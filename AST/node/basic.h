//
// Created by Sakura on 25-8-27.
//v

#ifndef BASIC_H
#define BASIC_H

class SemanticAnalyzer;


class LiteralExpression;
class PathExpression;
class BlockExpression;
class BorrowExpression;
class DereferenceExpression;
class NegateExpression;
class ArithmeticOrLogicalExpression;
class ComparisonExpression;
class LazyBooleanExpression;
class TypeCastExpression;
class AssignmentExpression;
class CompoundAssignmentExpression;
class GroupedExpression;
class ArrayExpression;
class IndexExpression;
class StructExpression;
class CallExpression;
class MethodCallExpression;
class FieldAccessExpression;
class InfiniteExpression;
class PredicateExpression;
class BreakExpression;
class ContinueExpression;
class IfExpression;
class ReturnExpression;
class UnderscoreExpression;

class TypePath;
class ReferenceType;
class ArrayType;
class UnitType;

class ProgramNode;
class FunctionItem;

enum astNodetype {
    Program,
};

class BasicNode {
protected:
    BasicNode(){}
    BasicNode(astNodetype type) : type(type) {}
public:
    virtual ~BasicNode() = default;
    astNodetype type;

    virtual void accept(SemanticAnalyzer &visitor) = 0;
};

class SemanticAnalyzer {
public:
    virtual void visit(BasicNode &node) = 0;

    //expression.h
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
    void visit(const ArrayExpression & node);
    void visit(const IndexExpression & node);
    void visit(const StructExpression & node);
    void visit(const CallExpression & node);
    void visit(const MethodCallExpression & node);
    void visit(const FieldAccessExpression & node);
    void visit(const InfiniteExpression & node);
    void visit(const PredicateExpression & node);
    void visit(const BreakExpression & node);
    void visit(const ContinueExpression & node);
    void visit(const IfExpression & node);
    void visit(const ReturnExpression & node);
    void visit(const UnderscoreExpression & node);

    //parser_type.h
    void visit(const TypePath & node);
    void visit(const ReferenceType & node);
    void visit(const ArrayType & node);
    void visit(const UnitType & node);

    //parser.h & parser.cpp
    void visit(const ProgramNode & node);
    void visit(const FunctionItem & node);
};

#endif //BASIC_H
