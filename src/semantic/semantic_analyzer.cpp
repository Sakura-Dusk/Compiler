//
// Created by Sakura on 25-10-19.
//

#include "semantic_analyzer.h"
#include "ast_visitor.h"
#include <sstream>
#include <iostream>
#include <memory>

SemanticAnalyzer::SemanticAnalyzer() : root(nullptr) {
    visitor = std::make_unique<SemanticVisitor>(this);
}

SemanticAnalyzer::SemanticAnalyzer(AstNode* ast_root) : root(ast_root) {
    visitor = std::make_unique<SemanticVisitor>(this);
}

SemanticAnalyzer::~SemanticAnalyzer() = default;

void SemanticAnalyzer::set_root(AstNode* ast_root) {
    root = ast_root;
}

void SemanticAnalyzer::analyze() {
    if (!root) {
        report_error("AST root is null", nullptr);
        return;
    }
    
    errors.clear();
    scope_stack.clear();
    
    // 创建全局作用域
    push_scope(root);
    
    // 添加预定义函数
    add_builtin_functions();
    
    try {
        // 使用统一的 visitor 进行分析
        visitor->visit(root);
    } catch (const SemanticError& e) {
        errors.push_back(e.what());
    }
    
    pop_scope();
}

void SemanticAnalyzer::push_scope(AstNode* node) {
    scope_stack.push_back(&node->scope_value);
}

void SemanticAnalyzer::pop_scope() {
    if (!scope_stack.empty()) {
        scope_stack.pop_back();
    }
}

Scope* SemanticAnalyzer::current_scope() {
    if (scope_stack.empty()) {
        return nullptr;
    }
    return scope_stack.back();
}

void SemanticAnalyzer::declare_variable(const std::string& name, NodeType* type, bool is_mutable, AstNode* node) {
    if (!current_scope()) {
        return;
    }
    
    scope_item item;
    item.type = *type;
    item.is_mutable = is_mutable;
    item.is_const = !is_mutable;
    
    current_scope()->item_table[name] = item;
}

scope_item* SemanticAnalyzer::lookup_variable(const std::string& name) {
    // 从当前作用域向外查找
    for (auto it = scope_stack.rbegin(); it != scope_stack.rend(); ++it) {
        auto found = (*it)->item_table.find(name);
        if (found != (*it)->item_table.end()) {
            return &found->second;
        }
    }
    return nullptr;
}

bool SemanticAnalyzer::is_type_compatible(NodeType* left, NodeType* right) {
    if (!left || !right) {
        return false;
    }
    
    // 完全相同
    if (*left == *right) {
        return true;
    }
    
    // 数组类型检查
    if (left->type == NodeTypeType::Array && right->type == NodeTypeType::Array) {
        // 检查元素类型是否一致
        if (!is_type_compatible(left->inside_type, right->inside_type)) {
            return false;
        }
        
        // 检查数组长度是否一致（对于已知长度的数组）
        // 如果两个数组都有确定的长度，则长度必须相同
        if (left->item_length > 0 && right->item_length > 0) {
            return left->item_length == right->item_length;
        }
        
        // 如果其中一个长度为0（未知长度），则认为是兼容的
        // 这种情况通常出现在类型推导中
        return true;
    }
    
    // 数值类型兼容性（简化处理）
    if ((left->type == NodeTypeType::I32 || left->type == NodeTypeType::U32 ||
         left->type == NodeTypeType::Isize || left->type == NodeTypeType::Usize) &&
        (right->type == NodeTypeType::I32 || right->type == NodeTypeType::U32 ||
         right->type == NodeTypeType::Isize || right->type == NodeTypeType::Usize)) {
        return true;
    }
    
    return false;
}

NodeType* SemanticAnalyzer::get_array_element_type(NodeType* array_type) {
    if (!array_type || array_type->type != NodeTypeType::Array) {
        return nullptr;
    }
    
    return array_type->inside_type;
}

void SemanticAnalyzer::report_error(const std::string& message, AstNode* node) {
    std::ostringstream oss;
    oss << "Semantic Error";
    if (node && node->father) {
        oss << " at line " << node->father->now_go_son_id;
    }
    oss << ": " << message;
    errors.push_back(oss.str());
}

std::vector<std::string> SemanticAnalyzer::get_errors() const {
    return errors;
}

bool SemanticAnalyzer::has_errors() const {
    return !errors.empty();
}

void SemanticAnalyzer::add_builtin_functions() {
    if (!current_scope()) {
        return;
    }
    
    // 添加 exit 函数
    NodeType exit_type;
    exit_type.type = NodeTypeType::Function;
    exit_type.inside_type = new NodeType(); // 使用 inside_type 作为返回类型
    exit_type.inside_type->type = NodeTypeType::Never; // exit 函数从不返回
    
    scope_item exit_item;
    exit_item.type = exit_type;
    exit_item.is_mutable = false;
    exit_item.is_const = true;
    
    current_scope()->item_table["exit"] = exit_item;
    
    // 添加 printInt 函数
    NodeType printInt_type;
    printInt_type.type = NodeTypeType::Function;
    printInt_type.inside_type = new NodeType(); // 使用 inside_type 作为返回类型
    printInt_type.inside_type->type = NodeTypeType::Unit; // printInt 返回 unit 类型
    
    scope_item printInt_item;
    printInt_item.type = printInt_type;
    printInt_item.is_mutable = false;
    printInt_item.is_const = true;
    
    current_scope()->item_table["printInt"] = printInt_item;
    
    // 可以添加更多内置函数...
}