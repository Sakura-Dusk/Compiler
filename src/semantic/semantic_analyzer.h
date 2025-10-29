//
// Created by Sakura on 25-10-19.
//

#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "../AST/node/basic.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

// 前向声明
class SemanticVisitor;

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
    std::unique_ptr<SemanticVisitor> visitor;
    
    // Visitor pattern 的友元类声明
    friend class SemanticVisitor;

public:
    // 作用域管理
    void push_scope(AstNode* node);
    void pop_scope();
    Scope* current_scope();
    
    // 符号表操作
    void declare_variable(const std::string& name, NodeType* type, bool is_mutable, AstNode* node);
    scope_item* lookup_variable(const std::string& name);
    
    // 辅助函数
    bool is_type_compatible(NodeType* left, NodeType* right);
    NodeType* get_array_element_type(NodeType* array_type);
    void report_error(const std::string& message, AstNode* node);

    SemanticAnalyzer();
    explicit SemanticAnalyzer(AstNode* ast_root);
    ~SemanticAnalyzer();
    
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