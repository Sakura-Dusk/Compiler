//
// Created by Sakura on 25-10-19.
//

#include "semantic_analyzer.h"
#include <sstream>
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() : root(nullptr) {}

SemanticAnalyzer::SemanticAnalyzer(AstNode* ast_root) : root(ast_root) {}

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
        // 分析程序
        if (root->type == AstNodetype::Program) {
            for (auto* child : root->children) {
                analyze_statement(child);
            }
        }
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

void SemanticAnalyzer::analyze_statement(AstNode* stmt_node) {
    if (!stmt_node) return;
    
    switch (stmt_node->type) {
        case AstNodetype::Function:
            // 函数声明 - 分析函数体
            // 函数的第三个子节点是函数体（Statements）
            if (stmt_node->children.size() >= 3) {
                analyze_statement(stmt_node->children[2]);
            }
            break;
        case AstNodetype::LetStatement:
            analyze_let_statement(stmt_node);
            break;
        case AstNodetype::Expression:
            analyze_expression(stmt_node->children[0]);
            break;
        case AstNodetype::Struct:
        case AstNodetype::Enumeration:
        case AstNodetype::ConstantItem:
            // 其他声明 - 暂时跳过
            break;
        default:
            // 其他语句类型
            for (auto* child : stmt_node->children) {
                analyze_statement(child);
            }
            break;
    }
}

void SemanticAnalyzer::analyze_let_statement(AstNode* let_node) {
    if (!let_node || let_node->type != AstNodetype::LetStatement) {
        return;
    }
    
    std::string var_name = let_node->value;
    
    // 检查是否已声明
    if (current_scope()->item_table.find(var_name) != current_scope()->item_table.end()) {
        report_error("Variable '" + var_name + "' already declared", let_node);
        return;
    }
    
    // 获取可变性
    bool is_mutable = (let_node->children[0]->value == "mut");
    
    // 分析类型
    NodeType* declared_type = nullptr;
    if (let_node->children.size() >= 2) {
        if (let_node->children[1]->type == AstNodetype::Type && 
            let_node->children[1]->value != "_") {
            declared_type = analyze_type(let_node->children[1]);
        }
    }
    
    // 分析初始化表达式
    NodeType* init_type = nullptr;
    if (let_node->children.size() >= 3) {
        init_type = analyze_expression(let_node->children[2]);
    }
    
    // 类型检查
    if (declared_type && init_type) {
        if (declared_type->type == NodeTypeType::Array) {
            // 数组类型特殊检查
            check_array_initialization(let_node);
            // 同时进行类型兼容性检查
            if (!is_type_compatible(declared_type, init_type)) {
                report_error("Type mismatch in variable initialization: cannot assign " +
                            init_type->show() + " to " + declared_type->show(), let_node);
            }
        } else if (!is_type_compatible(declared_type, init_type)) {
            report_error("Type mismatch in variable initialization", let_node);
        }
    }
    
    // 确定最终类型
    NodeType* final_type = declared_type ? declared_type : init_type;
    if (!final_type) {
        report_error("Cannot determine type of variable '" + var_name + "'", let_node);
        return;
    }
    
    // 声明变量
    declare_variable(var_name, final_type, is_mutable, let_node);
}

NodeType* SemanticAnalyzer::analyze_type(AstNode* type_node) {
    if (!type_node || type_node->type != AstNodetype::Type) {
        return nullptr;
    }
    
    auto* node_type = new NodeType();
    
    // 基本类型
    if (type_node->value == "i32") node_type->type = NodeTypeType::I32;
    else if (type_node->value == "u32") node_type->type = NodeTypeType::U32;
    else if (type_node->value == "isize") node_type->type = NodeTypeType::Isize;
    else if (type_node->value == "usize") node_type->type = NodeTypeType::Usize;
    else if (type_node->value == "bool") node_type->type = NodeTypeType::Bool;
    else if (type_node->value == "char") node_type->type = NodeTypeType::Char;
    else if (type_node->value == "str") node_type->type = NodeTypeType::Str;
    else if (type_node->value == "()") node_type->type = NodeTypeType::Unit;
    else if (type_node->value == "_") node_type->type = NodeTypeType::Wildcard;
    else {
        // 结构体或枚举类型
        node_type->type = NodeTypeType::Struct;
        node_type->SE_name = type_node->value;
    }
    
    // 数组类型 [Type; Length]
    if (!type_node->children.empty() &&
        type_node->children.size() >= 2) {
        node_type->type = NodeTypeType::Array;
        node_type->inside_type = analyze_type(type_node->children[0]);
        
        // 计算数组长度
        int length = evaluate_array_length(type_node->children[1]);
        if (length > 0) {
            node_type->item_length = length;
        } else {
            node_type->item_length = 0; // 将在具体初始化时设置
        }
    }
    
    // 引用类型 &Type 或 &mut Type
    if (!type_node->children.empty() && 
        type_node->children[0]->type == AstNodetype::Amp) {
        bool is_mut = false;
        if (type_node->children.size() > 1 && 
            type_node->children[1]->type == AstNodetype::Mut) {
            is_mut = true;
        }
        
        node_type->type = is_mut ? NodeTypeType::Mut_Amp : NodeTypeType::Amp;
        if (type_node->children.size() >= 2) {
            int type_child_index = is_mut ? 2 : 1;
            node_type->inside_type = analyze_type(type_node->children[type_child_index]);
        }
    }
    
    return node_type;
}

NodeType* SemanticAnalyzer::analyze_expression(AstNode* expr_node) {
    if (!expr_node) {
        return nullptr;
    }
    
    // 如果是 Expression 节点，递归分析其子节点
    if (expr_node->type == AstNodetype::Expression) {
        if (!expr_node->children.empty()) {
            return analyze_expression(expr_node->children[0]);
        }
        return nullptr;
    }
    
    switch (expr_node->type) {
        case AstNodetype::Identifier: {
            auto* var = lookup_variable(expr_node->value);
            if (!var) {
                report_error("Undefined variable '" + expr_node->value + "'", expr_node);
                return nullptr;
            }
            return new NodeType(var->type);
        }
        
        case AstNodetype::Integer_Literal: {
            auto* type = new NodeType();
            type->type = NodeTypeType::I32;
            return type;
        }
        
        case AstNodetype::Bool_Literal: {
            auto* type = new NodeType();
            type->type = NodeTypeType::Bool;
            return type;
        }
        
        case AstNodetype::ArrayElements: {
            // 数组字面量 [1, 2, 3] 或 [false; 5] 或 [[1,2], [3,4]]
            auto* type = new NodeType();
            type->type = NodeTypeType::Array;
            
            if (expr_node->children.empty()) {
                report_error("Empty array literal", expr_node);
                return nullptr;
            }
            
            // 检查是否是 [value; count] 语法 - 两个子节点，第二个是包含整数的表达式
            if (expr_node->children.size() == 2) {
                // 检查第二个子节点是否为包含 Integer_Literal 的 Expression
                if (expr_node->children[1]->type == AstNodetype::Expression &&
                    !expr_node->children[1]->children.empty() &&
                    expr_node->children[1]->children[0]->type == AstNodetype::Integer_Literal) {
                    NodeType* element_type = analyze_expression(expr_node->children[0]);
                    if (!element_type) {
                        return nullptr;
                    }
                    type->inside_type = element_type;
                    type->item_length = 0; // 长度由类型声明决定
                    return type;
                }
            }
            
            // 检查是否是 [value; count] 语法 - 单个子节点
            if (expr_node->children.size() == 1) {
                NodeType* element_type = analyze_expression(expr_node->children[0]);
                if (!element_type) {
                    return nullptr;
                }
                type->inside_type = element_type;
                type->item_length = 0; // 长度由类型声明决定
                return type;
            }
            
            // 分析第一个元素的类型
            NodeType* element_type = analyze_expression(expr_node->children[0]);
            if (!element_type) {
                return nullptr;
            }
            
            // 检查所有元素类型是否一致
            for (size_t i = 1; i < expr_node->children.size(); i++) {
                NodeType* current_type = analyze_expression(expr_node->children[i]);
                if (!is_type_compatible(element_type, current_type)) {
                    report_error("Array elements have different types", expr_node);
                    return nullptr;
                }
            }
            
            type->inside_type = element_type;
            type->item_length = expr_node->children.size();
            return type;
        }
        
        case AstNodetype::ArrayIndex: {
            // 数组索引 arr[index]
            if (expr_node->children.size() < 2) {
                report_error("Invalid array index expression", expr_node);
                return nullptr;
            }
            
            // 分析数组类型
            NodeType* array_type = analyze_expression(expr_node->children[0]);
            if (!array_type || array_type->type != NodeTypeType::Array) {
                report_error("Cannot index non-array type", expr_node);
                return nullptr;
            }
            
            // 分析索引类型
            NodeType* index_type = analyze_expression(expr_node->children[1]);
            if (!index_type || index_type->type != NodeTypeType::I32) {
                report_error("Array index must be integer", expr_node->children[1]);
                return nullptr;
            }
            
            check_array_index(expr_node);
            return get_array_element_type(array_type);
        }
        
        case AstNodetype::Binary_Operator: {
            if (expr_node->value == "=") {
                // 赋值表达式
                if (expr_node->children.size() < 2) {
                    report_error("Invalid assignment", expr_node);
                    return nullptr;
                }
                
                // 检查左值是否可变
                if (expr_node->children[0]->type == AstNodetype::Identifier) {
                    auto* var = lookup_variable(expr_node->children[0]->value);
                    if (!var) {
                        report_error("Undefined variable '" + expr_node->children[0]->value + "'", expr_node->children[0]);
                        return nullptr;
                    }
                    
                    if (!var->is_mutable) {
                        report_error("Cannot assign to immutable variable '" + expr_node->children[0]->value + "'", expr_node->children[0]);
                        return nullptr;
                    }
                } else if (expr_node->children[0]->type == AstNodetype::ArrayIndex) {
                    check_array_assignment(expr_node);
                }
                
                // 类型检查
                NodeType* left_type = analyze_expression(expr_node->children[0]);
                NodeType* right_type = analyze_expression(expr_node->children[1]);
                
                if (!is_type_compatible(left_type, right_type)) {
                    report_error("Type mismatch in assignment: cannot assign " +
                                right_type->show() + " to " + left_type->show(), expr_node);
                    return nullptr;
                }
                
                return left_type;
            }
            
            // 其他二元操作符
            if (expr_node->children.size() < 2) {
                return nullptr;
            }
            
            NodeType* left_type = analyze_expression(expr_node->children[0]);
            NodeType* right_type = analyze_expression(expr_node->children[1]);
            
            // 简化处理：假设所有二元操作都返回相同类型
            return left_type;
        }
        
        default:
            // 其他表达式类型
            for (auto* child : expr_node->children) {
                analyze_expression(child);
            }
            return nullptr;
    }
}

void SemanticAnalyzer::check_array_initialization(AstNode* let_node) {
    if (!let_node || let_node->type != AstNodetype::LetStatement) {
        return;
    }
    
    // 获取声明的数组类型
    NodeType* declared_type = nullptr;
    if (let_node->children.size() >= 2) {
        declared_type = analyze_type(let_node->children[1]);
    }
    
    if (!declared_type || declared_type->type != NodeTypeType::Array) {
        return;
    }
    
    // 获取初始化表达式
    if (let_node->children.size() < 3) {
        return;
    }
    
    AstNode* init_expr = let_node->children[2];
    if (init_expr->children.empty()) {
        return;
    }
    
    AstNode* array_elements = init_expr->children[0];
    if (array_elements->type != AstNodetype::ArrayElements) {
        return;
    }
    
    // 检查数组长度
    int expected_length = declared_type->item_length;
    
    if (expected_length > 0) {
        if (!check_array_element_count(array_elements, expected_length)) {
            // 计算实际长度
            int actual_length = array_elements->children.size();
            if (array_elements->children.size() == 2) {
                if (array_elements->children[1]->type == AstNodetype::Expression &&
                    !array_elements->children[1]->children.empty() &&
                    array_elements->children[1]->children[0]->type == AstNodetype::Integer_Literal) {
                    try {
                        actual_length = std::stoi(array_elements->children[1]->children[0]->value);
                    } catch (...) {
                        actual_length = array_elements->children.size();
                    }
                } else if (array_elements->children[1]->type == AstNodetype::Integer_Literal) {
                    try {
                        actual_length = std::stoi(array_elements->children[1]->value);
                    } catch (...) {
                        actual_length = array_elements->children.size();
                    }
                }
            }
            
            report_error("Array length mismatch: expected " + std::to_string(expected_length) +
                        " elements, got " + std::to_string(actual_length), let_node);
            return;
        }
    }
    
    // 检查元素类型
    if (declared_type->inside_type) {
        // 对于 [value; count] 语法，只检查第一个元素
        if (array_elements->children.size() == 2 &&
            array_elements->children[1]->type == AstNodetype::Expression &&
            !array_elements->children[1]->children.empty() &&
            array_elements->children[1]->children[0]->type == AstNodetype::Integer_Literal) {
            // 只检查第一个元素类型
            NodeType* element_type = analyze_expression(array_elements->children[0]);
            if (!is_type_compatible(declared_type->inside_type, element_type)) {
                report_error("Array element type mismatch", array_elements->children[0]);
                return;
            }
        } else {
            // 普通数组语法，检查所有元素
            for (auto* element : array_elements->children) {
                NodeType* element_type = analyze_expression(element);
                if (!is_type_compatible(declared_type->inside_type, element_type)) {
                    report_error("Array element type mismatch", element);
                    return;
                }
                
                // 对于多维数组，检查内层数组的长度
                if (declared_type->inside_type->type == NodeTypeType::Array &&
                    element_type->type == NodeTypeType::Array) {
                    int expected_inner_length = declared_type->inside_type->item_length;
                    int actual_inner_length = element_type->item_length;
                    
                    if (expected_inner_length > 0 && actual_inner_length != expected_inner_length) {
                        report_error("Multi-dimensional array inner length mismatch: expected " +
                                    std::to_string(expected_inner_length) + " elements, got " +
                                    std::to_string(actual_inner_length), element);
                        return;
                    }
                }
            }
        }
    }
    
    // 更新实际数组长度
    // 对于 [value; count] 语法，从第二个子节点获取长度
    if (array_elements->children.size() == 2 &&
        array_elements->children[1]->type == AstNodetype::Expression &&
        !array_elements->children[1]->children.empty() &&
        array_elements->children[1]->children[0]->type == AstNodetype::Integer_Literal) {
        // 这是 [value; count] 语法，从 count 获取实际长度
        try {
            int count = std::stoi(array_elements->children[1]->children[0]->value);
            declared_type->item_length = count;
        } catch (...) {
            // 如果解析失败，保持原样
        }
    } else {
        declared_type->item_length = array_elements->children.size();
    }
}

void SemanticAnalyzer::check_array_index(AstNode* index_node) {
    if (!index_node || index_node->type != AstNodetype::ArrayIndex) {
        return;
    }
    
    // 检查数组是否存在
    if (index_node->children.empty()) {
        return;
    }
    
    AstNode* array_expr = index_node->children[0];
    if (array_expr->type == AstNodetype::Identifier) {
        auto* var = lookup_variable(array_expr->value);
        if (!var) {
            report_error("Undefined array '" + array_expr->value + "'", array_expr);
            return;
        }
        
        if (var->type.type != NodeTypeType::Array) {
            report_error("'" + array_expr->value + "' is not an array", array_expr);
            return;
        }
    }
}

void SemanticAnalyzer::check_array_assignment(AstNode* assign_node) {
    if (!assign_node || assign_node->type != AstNodetype::Binary_Operator || 
        assign_node->value != "=") {
        return;
    }
    
    if (assign_node->children.empty()) {
        return;
    }
    
    AstNode* left_expr = assign_node->children[0];
    if (left_expr->type != AstNodetype::ArrayIndex) {
        return;
    }
    
    // 检查数组是否可变
    if (left_expr->children.empty()) {
        return;
    }
    
    AstNode* array_expr = left_expr->children[0];
    if (array_expr->type == AstNodetype::Identifier) {
        auto* var = lookup_variable(array_expr->value);
        if (!var) {
            report_error("Undefined array '" + array_expr->value + "'", array_expr);
            return;
        }
        
        if (!var->is_mutable) {
            report_error("Cannot modify immutable array '" + array_expr->value + "'", array_expr);
            return;
        }
    }
}

int SemanticAnalyzer::evaluate_array_length(AstNode* length_expr) {
    // 简化处理：只支持整数常量
    if (!length_expr || length_expr->children.empty()) {
        return -1;
    }
    
    AstNode* literal = length_expr->children[0];
    if (literal->type == AstNodetype::Integer_Literal) {
        try {
            return std::stoi(literal->value);
        } catch (const std::exception&) {
            return -1;
        }
    }
    
    return -1;
}

bool SemanticAnalyzer::check_array_element_count(AstNode* array_elements, int expected_length) {
    if (!array_elements || array_elements->type != AstNodetype::ArrayElements) {
        return false;
    }
    
    // 检查是否是重复语法 [value; count]
    if (array_elements->children.size() == 2) {
        // 检查第二个子节点是否为包含 Integer_Literal 的 Expression
        if (array_elements->children[1]->type == AstNodetype::Expression &&
            !array_elements->children[1]->children.empty() &&
            array_elements->children[1]->children[0]->type == AstNodetype::Integer_Literal) {
            // 这是 [value; count] 语法，检查 count 是否匹配 expected_length
            try {
                int count = std::stoi(array_elements->children[1]->children[0]->value);
                return count == expected_length;
            } catch (...) {
                return false;
            }
        }
        
        // 检查第二个子节点是否直接是 Integer_Literal
        if (array_elements->children[1]->type == AstNodetype::Integer_Literal) {
            try {
                int count = std::stoi(array_elements->children[1]->value);
                return count == expected_length;
            } catch (...) {
                return false;
            }
        }
    }
    
    // 检查是否是单元素重复语法 [value]
    if (array_elements->children.size() == 1) {
        // 这种情况需要从类型声明中获取长度
        return true;
    }
    
    return array_elements->children.size() == expected_length;
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
    
    // 可以添加更多内置函数...
}