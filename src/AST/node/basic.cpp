//
// Created by Sakura on 25-9-22.
//

#include "basic.h"

AstNode::~AstNode() {
  for (auto child : children) {
    delete child;
  }
  children.clear();
}

std::vector<std::string> AstNode::show_node() const {
    std::vector<std::string> res;
    res.emplace_back("");
    if (type == AstNodetype::Amp) res.back() = "Amp";
    if (type == AstNodetype::Break) res.back() = "Break";
    if (type == AstNodetype::Continue) res.back() = "Continue";
    if (type == AstNodetype::Else) res.back() = "Else";
    if (type == AstNodetype::Enumeration) res.back() = "Enumeration";
    if (type == AstNodetype::Expression) res.back() = "Expression";
    if (type == AstNodetype::Field) res.back() = "Field";
    if (type == AstNodetype::Fields) res.back() = "Fields";
    if (type == AstNodetype::Function) res.back() = "Function";
    if (type == AstNodetype::Identifier) res.back() = "Identifier";
    if (type == AstNodetype::If) res.back() = "If";
    if (type == AstNodetype::Implementation) res.back() = "Implementation";
    if (type == AstNodetype::Loop) res.back() = "Loop";
    if (type == AstNodetype::Mut) res.back() = "Mut";
    if (type == AstNodetype::Pattern) res.back() = "Pattern";
    if (type == AstNodetype::Program) res.back() = "Program";
    if (type == AstNodetype::Return) res.back() = "Return";
    if (type == AstNodetype::Self) res.back() = "Self";
    if (type == AstNodetype::Statements) res.back() = "Statements";
    if (type == AstNodetype::Struct) res.back() = "Struct";
    if (type == AstNodetype::Trait) res.back() = "Trait";
    if (type == AstNodetype::Type) res.back() = "Type";
    if (type == AstNodetype::While) res.back() = "While";
    if (type == AstNodetype::ArrayElements) res.back() = "ArrayElements";
    if (type == AstNodetype::ArrayIndex) res.back() = "ArrayIndex";
    if (type == AstNodetype::AssociatedItem) res.back() = "AssociatedItem";
    if (type == AstNodetype::Binary_Operator) res.back() = "Binary_Operator";
    if (type == AstNodetype::Bool_Literal) res.back() = "Bool_Literal";
    if (type == AstNodetype::CallParams) res.back() = "CallParams";
    if (type == AstNodetype::Char_Literal) res.back() = "Char_Literal";
    if (type == AstNodetype::ConstantItem) res.back() = "ConstantItem";
    if (type == AstNodetype::Enumeration_Items) res.back() = "Enumeration_Items";
    if (type == AstNodetype::Float_Literal) res.back() = "Float_Literal";
    if (type == AstNodetype::FunctionCall) res.back() = "FunctionCall";
    if (type == AstNodetype::FunctionParameters) res.back() = "FunctionParameters";
    if (type == AstNodetype::Integer_Literal) res.back() = "Integer_Literal";
    if (type == AstNodetype::LetStatement) res.back() = "LetStatement";
    if (type == AstNodetype::String_Literal) res.back() = "String_Literal";
    if (type == AstNodetype::StructField) res.back() = "StructField";
    if (type == AstNodetype::StructParameters) res.back() = "StructParameters";
    if (type == AstNodetype::TypedIdentifier) res.back() = "TypedIdentifier";
    if (type == AstNodetype::Unary_Operator) res.back() = "Unary_Operator";

    if (!value.empty()) res.back() += ": " + value;
    return res;
}

// NodeType implementation
NodeType::NodeType() : type(NodeTypeType::Unknown), inside_type(nullptr), self_type(nullptr),
                       is_mutable(false), item_length(0), FM_id(0), field(nullptr) {}

NodeType::NodeType(const NodeTypeType& t) : type(t), inside_type(nullptr), self_type(nullptr),
                                           is_mutable(false), item_length(0), FM_id(0), field(nullptr) {}

NodeType::NodeType(const NodeTypeType &t, NodeType *fa_t, const int &x) : type(t), inside_type(fa_t), self_type(nullptr),
                     is_mutable(false), item_length(x), FM_id(0), field(nullptr) {}

std::string NodeType::show() const {
    std::string res;
    switch (type) {
        case NodeTypeType::I32: return "(i32)";
        case NodeTypeType::U32: return "(u32)";
        case NodeTypeType::Isize: return "(isize)";
        case NodeTypeType::Usize: return "(usize)";
        case NodeTypeType::AllInt: return "(int)";
        case NodeTypeType::IInt: return "(iint)";
        case NodeTypeType::UInt: return "(uint)";
        case NodeTypeType::Bool: return "(bool)";
        case NodeTypeType::Char: return "(char)";
        case NodeTypeType::Str: return "(str)";
        case NodeTypeType::String: return "(String)";
        case NodeTypeType::Unit: return "()[Unit]";
        case NodeTypeType::Struct:
            res = "(struct){" + SE_name;
            bool first = true;
            for (auto& [name, field_type] : field->item_table) {
                if (first == false) res += ",";
                if (field_type.is_mutable) res += " [mutable]";
                res += " " + name + ": " + field_type.type.show();
                first = false;
            }
            res += "}";
            return res;
        case NodeTypeType::Enum: return "(enum){" + SE_name + "}";
        case NodeTypeType::Array: {
            res = "(Array)[";
            if (inside_type) res += inside_type->show();
            res += "; " + std::to_string(item_length) + "]";
            return res;
        }
        case NodeTypeType::Function:
            res = "(Function)(";
            bool firs = true;
            for (int i = 0; i < items_type.size(); i++) {
                if (firs == false) res += ", ";
                res += items_type[i]->show();
                firs = false;
            }
            res += ") -> (" + inside_type->show() + ")";
            return res;
        case NodeTypeType::Method:
            res = self_type->show() + ".(";
            bool fir = true;
            for (int i = 0; i < items_type.size(); i++) {
                if (fir == false) res += ", ";
                res += items_type[i]->show();
                fir = false;
            }
            res += ") -> (" + inside_type->show() + ")";
            return res;
        case NodeTypeType::Type_of_Type: return "(" + inside_type->show() + ")_Type";
        case NodeTypeType::Amp: {
            std::string result = "&";
            if (inside_type) result += inside_type->show();
            return result;
        }
        case NodeTypeType::Mut_Amp: {
            std::string result = "&mut ";
            if (inside_type) result += inside_type->show();
            return result;
        }
        case NodeTypeType::Wildcard: return "_[wildcard]";
        case NodeTypeType::Never: return "![never]";
        case NodeTypeType::Unknown: return "unknown";
        default: return "really unknown !!!";
    }
}

bool NodeType::operator==(const NodeType& other) const {
    if (type != other.type) {
        return false;
    }
    
    switch (type) {
        case NodeTypeType::Struct:
        case NodeTypeType::Enum:
            return SE_name == other.SE_name;
        case NodeTypeType::Array:
            return item_length == other.item_length &&
                   (inside_type && other.inside_type && *inside_type == *other.inside_type);
        case NodeTypeType::Function:
            bool res = inside_type && other.inside_type && *inside_type == *other.inside_type &&
                       items_type.size() == other.items_type.size();
            for (int i = 0; i < items_type.size(); i++)
                res &= items_type[i] == other.items_type[i];
            return res;
        case NodeTypeType::Type_of_Type:
            return inside_type && other.inside_type && *inside_type == *other.inside_type;
        case NodeTypeType::Amp:
        case NodeTypeType::Mut_Amp:
            return inside_type && other.inside_type && *inside_type == *other.inside_type;
        default:
            return true;
    }
}

std::vector<std::string> AstNode::show_tree() const {
    auto res = show_node();
    for (int i = 0; i < children.size(); i++) {
        auto son_res = children[i]->show_tree();
        res.emplace_back("|");
        res.push_back((i == children.size() - 1 ? "L__" : "|__") + son_res[0]);
        for (int j = 1; j < son_res.size(); j++) {
            if (i != children.size() - 1) res.push_back("|  " + son_res[j]);
                else res.push_back(son_res[j]);
        }
    }
    return res;
}

scope_item::scope_item(NodeType type, std::any const_value, const bool& is_mutable, const bool& is_uncoverable, const long long ID = -1)
    : type(std::move(type)), const_value(std::move(const_value)), is_mutable(is_mutable), is_uncoverable(is_uncoverable), ID(ID) {}

scope_item unknown_item = scope_item(NodeTypeType::Unknown, std::any(), false, false);

scope_item &Scope::get_item(const std::string &name) {
    if (item_table.contains(name)) return item_table[name];
    return unknown_item;
}

scope_item &Scope::get_type(const std::string &name) {
    if (type_table.contains(name)) return type_table[name];
    return unknown_item;
}

void Scope::add_item(const std::string &name, const scope_item &item) {
    item_table[name] = item;
}

void Scope::add_type(const std::string &name, const scope_item &item) {
    type_table[name] = item;
}
