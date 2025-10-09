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
    StructParameters,
    Identifier,
    AssociatedItem,
    Self,//self , Self is type
    Amp,
    Mut,
    TypedIdentifier,
    Type,
    Statements,
    LetStatement,
    Pattern,
    Expression,
    ArrayElements,
    Loop,
    While,
    If,
    Else,
    Unary_Operator,
    Binary_Operator,
    Char_Literal,
    Integer_Literal,
    String_Literal,
    Float_Literal,
    Bool_Literal,
    Break,
    Continue,
    Return,
    FunctionCall,
    ArrayIndex,
    StructField,
    Field,
    Fields,
    CallParams,
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
