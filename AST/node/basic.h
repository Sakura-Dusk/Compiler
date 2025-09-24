//
// Created by Sakura on 25-8-27.
//v

#ifndef BASIC_H
#define BASIC_H

#include<vector>
#include<string>

enum AstNodetype {
    Program,
    Function,
    Struct,
    Enumeration,
    Enumeration_Items,
    ConstantItem,
    Trait,
    Implementation,
    FunctionParameters,
    Identifier,
    AssociatedItem,
    self,
    Self,
    Amp,
    Mut,
};

class AstNode final {
public:
    ~AstNode();
    std::vector<AstNode*> children;
    AstNodetype type;
    std::string value;

    std::vector<std::string> show_tree() const;
    std::vector<std::string> show_node() const;
};

#endif //BASIC_H
