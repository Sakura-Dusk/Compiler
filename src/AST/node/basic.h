//
// Created by Sakura on 25-8-27.
//v

#ifndef BASIC_H
#define BASIC_H

#include <any>
#include <map>
#include <utility>
#include<vector>
#include<string>

enum class NodeTypeType {
    I32, U32, Isize, Usize,
    AllInt, IInt, UInt,
    Bool, Char, Str, String,
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
    std::map<std::string, unsigned int>* items_index;
    Scope* field = nullptr;

    bool is_exit = false;

    std::string show() const;

    NodeType();
    NodeType(const NodeTypeType&);
    NodeType(const NodeTypeType&, NodeType*, const int&);
    bool operator==(const NodeType &) const;
};

class scope_item {
public:
    scope_item() = default;
    scope_item(NodeType, std::any, const bool&, const bool&, long long);

    NodeType type = NodeTypeType::Unknown;
    std::any const_value;
    bool is_mutable = false;
    bool is_uncoverable = false;
    long long ID;
};

class Scope {
public:
    std::map<std::string, scope_item> item_table, type_table;

    scope_item& get_item(const std::string&);
    scope_item& get_type(const std::string&);

    void add_item(const std::string&, const scope_item&);
    void add_type(const std::string&, const scope_item&);
};

enum class AstNodetype {
    Program,
    Function,
    Struct,
    Enumeration,
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
    NodeType actual_type;
    std::any const_value;

    Scope* scope_value;
    AstNode* scope_father;

    int now_go_son_id;
    bool is_mut = false;
    bool is_variable = false;
    bool exist_break = false, exist_return = false, must_break = false;

    long long variableID;

    std::vector<std::string> show_tree() const;
    std::vector<std::string> show_node() const;
};

inline NodeType I32 = NodeType(NodeTypeType::I32);
inline NodeType U32 = NodeType(NodeTypeType::U32);
inline NodeType Isize = NodeType(NodeTypeType::Isize);
inline NodeType Usize = NodeType(NodeTypeType::Usize);
inline NodeType Int = NodeType(NodeTypeType::AllInt);
inline NodeType AllInt = NodeType(NodeTypeType::AllInt);
inline NodeType IInt = NodeType(NodeTypeType::IInt);
inline NodeType UInt = NodeType(NodeTypeType::UInt);
inline NodeType Bool = NodeType(NodeTypeType::Bool);
inline NodeType Char = NodeType(NodeTypeType::Char);
inline NodeType Str = NodeType(NodeTypeType::Str);
inline NodeType String = NodeType(NodeTypeType::String);
inline NodeType Unit = NodeType(NodeTypeType::Unit);
inline NodeType Struct = NodeType(NodeTypeType::Struct);
inline NodeType Enum = NodeType(NodeTypeType::Enum);
inline NodeType Array = NodeType(NodeTypeType::Array);
inline NodeType Function = NodeType(NodeTypeType::Function);
inline NodeType Method = NodeType(NodeTypeType::Method);
// inline NodeType Type_of_Type = NodeType(NodeTypeType::Type_of_Type);
inline NodeType Amp = NodeType(NodeTypeType::Amp);
inline NodeType Mut_Amp = NodeType(NodeTypeType::Mut_Amp);
inline NodeType Wildcard = NodeType(NodeTypeType::Wildcard);
inline NodeType Never = NodeType(NodeTypeType::Never);
inline NodeType UnKnown = NodeType(NodeTypeType::Unknown);

inline NodeType I32_Type = NodeType(NodeTypeType::Type_of_Type, &I32, 0);
inline NodeType U32_Type = NodeType(NodeTypeType::Type_of_Type, &U32, 0);
inline NodeType Isize_Type = NodeType(NodeTypeType::Type_of_Type, &Isize, 0);
inline NodeType Usize_Type = NodeType(NodeTypeType::Type_of_Type, &Usize, 0);
inline NodeType Bool_Type = NodeType(NodeTypeType::Type_of_Type, &Bool, 0);
inline NodeType Char_Type = NodeType(NodeTypeType::Type_of_Type, &Char, 0);
inline NodeType Str_Type = NodeType(NodeTypeType::Type_of_Type, &Str, 0);
inline NodeType String_Type = NodeType(NodeTypeType::Type_of_Type, &String, 0);
inline NodeType Wildcard_Type = NodeType(NodeTypeType::Type_of_Type, &Wildcard, 0);

inline NodeType Str_Amp = NodeType(NodeTypeType::Amp, &Str, 0);

#endif //BASIC_H
