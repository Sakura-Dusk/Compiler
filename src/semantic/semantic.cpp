//
// Created by Sakura on 26-1-12.
//

#include "semantic.h"

void semantic_visit_node(AstNode* node) {
    //TODO: implement semantic analysis visit
}

void build_universe_scope(AstNode* node) {
    node->scope_value = new Scope();
    node->scope_father = nullptr;

    NodeType funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.items_type.push_back(&I32);
    node->scope_value->add_item("exit", scope_item(funct, std::any(), false, true));
    node->scope_value->add_type("printInt", scope_item(funct, std::any(), false, true));
    node->scope_value->add_type("printlnInt", scope_item(funct, std::any(), false, true));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.items_type.push_back(&Str_Amp);
    node->scope_value->add_type("print", scope_item(funct, std::any(), false, true));
    node->scope_value->add_type("println", scope_item(funct, std::any(), false, true));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.inside_type = &I32;
    node->scope_value->add_type("getInt", scope_item(funct, std::any(), false, true));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.inside_type = &String;
    node->scope_value->add_type("getInt", scope_item(funct, std::any(), false, true));
}

void Semantic_check(AstNode* node) {
    build_universe_scope(node);
    semantic_visit_node(node);
}