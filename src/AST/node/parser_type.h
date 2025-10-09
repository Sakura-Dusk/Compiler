//
// Created by Sakura on 25-9-19.
//

#ifndef PARSER_TYPE_H
#define PARSER_TYPE_H
#include <utility>

#include "basic.h"
#include "expression.h"

class TypeNode : BasicNode {
    void accept(SemanticAnalyzer &visitor) override = 0;
};

class PathIdentSegment {
public:
    bool is_self;
    bool is_Self;
    std::string identifier;

    PathIdentSegment() : is_self(false), is_Self(false) {}
    PathIdentSegment(bool is_self, bool is_Self, std::string identifier) : is_self(is_self), is_Self(is_Self), identifier(std::move(identifier)) {}
};

class TypePath final : public TypeNode {
public:
    PathIdentSegment segment;
    bool hav_path;
    PathIdentSegment path;
    //path::segment  or segment

    TypePath(PathIdentSegment segment, bool hav_path, PathIdentSegment path) : segment(std::move(segment)), hav_path(hav_path), path(std::move(path)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class ReferenceType final : public TypeNode {
public:
    bool is_mut;
    std::shared_ptr<TypeNode> type;

    ReferenceType(bool is_mut, std::shared_ptr<TypeNode> type) : is_mut(is_mut), type(std::move(type)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class ArrayType final : public TypeNode {
public:
    std::shared_ptr<TypeNode> type;
    std::shared_ptr<Expression> expression;

    ArrayType(std::shared_ptr<TypeNode> type, std::shared_ptr<Expression> expression) : type(std::move(type)), expression(std::move(expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class UnitType final : public TypeNode {
public:
    UnitType() = default;

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

#endif //PARSER_TYPE_H
