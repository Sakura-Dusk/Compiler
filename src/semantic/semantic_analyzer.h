//
// Created by Sakura on 25-10-19.
//

#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "../AST/node/basic.h"
#include <string>
#include <vector>
#include <stdexcept>

class SemanticError : public std::runtime_error {
public:
    int line_number;
    int column_number;
    
    SemanticError(const std::string& message, int line = -1, int column = -1)
        : std::runtime_error(message), line_number(line), column_number(column) {}
};

class SemanticAnalyzer {
private:
    AstNode* root;
    std::vector<Scope*> scope_stack;
    
    // 作用域管理
    void push_scope(AstNode* node);
    void pop_scope();
    Scope* current_scope();
    
    // 类型检查
    NodeType* analyze_type(AstNode* type_node);
    NodeType* analyze_expression(AstNode* expr_node);
    void analyze_statement(AstNode* stmt_node);
    void analyze_let_statement(AstNode* let_node);
    void analyze_array_elements(AstNode* array_node, NodeType* expected_type);
    
    // 数组相关检查
    void check_array_initialization(AstNode* let_node);
    void check_array_index(AstNode* index_node);
    void check_array_assignment(AstNode* assign_node);
    int evaluate_array_length(AstNode* length_expr);
    bool check_array_element_count(AstNode* array_elements, int expected_length);
    
    // 符号表操作
    void declare_variable(const std::string& name, NodeType* type, bool is_mutable, AstNode* node);
    scope_item* lookup_variable(const std::string& name);
    
    // 辅助函数
    bool is_type_compatible(NodeType* left, NodeType* right);
    NodeType* get_array_element_type(NodeType* array_type);
    void report_error(const std::string& message, AstNode* node);

public:
    SemanticAnalyzer();
    explicit SemanticAnalyzer(AstNode* ast_root);
    
    void analyze();
    void set_root(AstNode* ast_root);
    
    // 错误处理
    std::vector<std::string> get_errors() const;
    bool has_errors() const;
    
    void add_builtin_functions();

private:
    std::vector<std::string> errors;
};

#endif //SEMANTIC_ANALYZER_H