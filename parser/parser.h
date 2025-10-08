#include "../AST/node/basic.h"
#include "../lexer.h"

class Parser {
    Lexer lexer;
    void parser_program(AstNode*);
    void parser_Item(AstNode*);
    void parser_Function(AstNode*);
    void parser_Struct(AstNode*);
    void parser_Enumeration(AstNode*);
    void parser_ConstantItem(AstNode*);
    void parser_Trait(AstNode*);
    void parser_Implementation(AstNode*);
    void parser_AssociatedItem(AstNode*);
    void parser_FunctionParameters(AstNode*);
    void parser_TypedIdentifier(AstNode*, bool);
    void parser_StructParameters(AstNode*);
    void parser_Type(AstNode*);
    void parser_Statements(AstNode*);
    void parser_LetStatement(AstNode*);
    void parser_Expression(AstNode*, bool only_flag = false);
    AstNode* pratt_Expression(int precedence, bool);
    void parser_ArrayElements(AstNode*);
    void parser_CallParams(AstNode*);

public:
    Parser();
    explicit Parser(const std::string&);
    AstNode* work();
};
