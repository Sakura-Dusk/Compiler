//
// Created by Sakura on 25-8-27.
//v

#ifndef BASIC_H
#define BASIC_H

#include <any>
#include <map>
#include<vector>
#include<string>

enum class NodeTypeType {
    I32, U32, Isize, Usize,
    AllInt, IInt, UInt,
    Bool, Char, Str,
    Unit,
    Struct, Enum, Array, Function, Method,
    Type_of_Type,//eg: i32, u32(for "as i32")
    Amp, Mut_Amp,//& , &mut
    Wildcard,//_
    Never,
    Unknown,
};

class Scope;

class NodeType {
public:
    NodeTypeType type;
    NodeType* inside_type;//for array each type, type_of_type inside type, function,method return type
    NodeType* self_type;//only for method, type of Self
    bool is_mutable = false;
    int item_length = 0, FM_id = 0;//function or method id
    std::string SE_name;//struct or enum name
    std::vector<NodeType*> items_type;
    std::map<std::string, int> items_index;
    Scope* scope;

    std::string show();

    NodeType();
    NodeType(NodeTypeType);
    bool operator==(const NodeType &) const;
};

class scope_item {
public:
    NodeType type;
    std::any const_value;
    bool is_mutable = false;
    bool is_const = false;
    int index = 0;
};

class Scope {
public:
    std::map<std::string, scope_item> item_table, type_table;
};

enum class AstNodetype {
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

    AstNode* father;
    std::any const_value;
    AstNode* scope_father;
    Scope scope_value;
    NodeType actual_type;
    int now_go_son_id;
    bool is_mut = false;
    bool is_variable = false;
    bool exist_break = false, exist_return = false, must_break = false;

    std::vector<std::string> show_tree() const;
    std::vector<std::string> show_node() const;
};


#endif //BASIC_H
