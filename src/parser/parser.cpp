#include "parser.h"
#include<iostream>

Parser::Parser():lexer("") {}

Parser::Parser(const std::string &code) : lexer(code) {}

AstNode* Parser::work() {
    const auto node = new AstNode();
    parser_program(node);
    return node;
}

void Parser::parser_program(AstNode *node) {
    node->type = AstNodetype::Program;
    while (lexer.peek_next_token().type != TokenType::Unknown) parser_Item(node);
    if (!lexer.is_End()) throw std::runtime_error("RE");
}

void Parser::parser_Item(AstNode *node) {
    auto token = lexer.peek_next_token();

    if (token.type != TokenType::Keyword) throw std::runtime_error("RE");
    if (token.value == "fn") {//function
        auto new_node = new AstNode;
        node->children.push_back(new_node);
        parser_Function(new_node);
    }
    else if (token.value == "struct") {
        auto new_node = new AstNode;
        node->children.push_back(new_node);
        parser_Struct(new_node);
    }
    else if (token.value == "enum") {
        auto new_node = new AstNode;
        node->children.push_back(new_node);
        parser_Enumeration(new_node);
    }
    else if (token.value == "const") {
        auto new_node = new AstNode;
        node->children.push_back(new_node);
        parser_ConstantItem(new_node);
    }
    else if (token.value == "trait") {
        auto new_node = new AstNode;
        node->children.push_back(new_node);
        parser_Trait(new_node);
    }
    else if (token.value == "impl") {
        auto new_node = new AstNode;
        node->children.push_back(new_node);
        parser_Implementation(new_node);
    }
    else throw std::runtime_error("RE");
}

void Parser::parser_Function(AstNode *node) {
    node->type = AstNodetype::Function;

    lexer.expect({TokenType::Keyword, "fn"});
    node->value = lexer.expect(TokenType::Identifier).value;

    //FunctionParameters
    lexer.expect({TokenType::Punctuation, "("});
    auto *new_node = new AstNode;
    parser_FunctionParameters(new_node);
    node->children.push_back(new_node);
    lexer.expect({TokenType::Punctuation, ")"});

    //FunctionReturnType
    auto token = lexer.peek_next_token();
    new_node = new AstNode;
    if (token == (Token){TokenType::Operator, "->"}) {
        lexer.get_next_token();
        parser_Type(new_node);
    }
    else {
        new_node->type = AstNodetype::Type;
        new_node->value = "()";
    }
    node->children.push_back(new_node);

    //BlockExpression or ;
    token = lexer.peek_next_token();
    if (token == (Token){TokenType::Punctuation, ";"}) {
        lexer.get_next_token();
    }
    else {
        lexer.expect({TokenType::Punctuation, "{"});
        new_node = new AstNode;
        parser_Statements(new_node);
        node->children.push_back(new_node);
        lexer.expect({TokenType::Punctuation, "}"});
    }
}

void Parser::parser_Struct(AstNode *node) {
    node->type = AstNodetype::Struct;

    lexer.expect({TokenType::Keyword, "struct"});
    node->value = lexer.expect(TokenType::Identifier).value;

    auto token = lexer.peek_next_token();
    if (token == (Token){TokenType::Punctuation, ";"}) {
        lexer.get_next_token();
    }
    else {
        lexer.expect({TokenType::Punctuation, "{"});
        auto new_node = new AstNode;
        new_node->type = AstNodetype::StructParameters;
        node->children.push_back(new_node);
        parser_StructParameters(new_node);
        lexer.expect({TokenType::Punctuation, "}"});
    }
}

void Parser::parser_Enumeration(AstNode *node) {
    node->type = AstNodetype::Enumeration;

    lexer.expect({TokenType::Keyword, "enum"});
    node->value = lexer.expect(TokenType::Identifier).value;

    lexer.expect({TokenType::Punctuation, "{"});

    bool is_comma = true;
    while (lexer.peek_next_token() != (Token){TokenType::Punctuation, "}"}) {
        if (is_comma == false) throw std::runtime_error("RE");
        is_comma = false;
        auto new_node = new AstNode;
        new_node->type = AstNodetype::Identifier;
        new_node->value = lexer.expect(TokenType::Identifier).value;
        node->children.push_back(new_node);

        if (lexer.peek_next_token() == (Token){TokenType::Punctuation, ","}) {
            is_comma = true;
            lexer.get_next_token();
        }
    }

    lexer.expect({TokenType::Punctuation, "}"});
}

void Parser::parser_ConstantItem(AstNode *node) {
   node->type = AstNodetype::ConstantItem;

    lexer.expect({TokenType::Keyword, "const"});
    node->value = lexer.expect(TokenType::Identifier).value;

    lexer.expect({TokenType::Punctuation, ":"});
    auto new_node = new AstNode;
    parser_Type(new_node);
    node->children.push_back(new_node);

    if (lexer.peek_next_token() == (Token){TokenType::Punctuation, ";"}) {
        lexer.get_next_token();
        return ;
    }

    lexer.expect({TokenType::Operator, "="});
    new_node = new AstNode;
    parser_Expression(new_node);
    node->children.push_back(new_node);
    lexer.expect({TokenType::Punctuation, ";"});
}

void Parser::parser_Trait(AstNode *node) {
    node->type = AstNodetype::Trait;
    lexer.expect({TokenType::Keyword, "trait"});
    node->value = lexer.expect(TokenType::Identifier).value;

    lexer.expect({TokenType::Punctuation, "{"});
    auto new_node = new AstNode;
    node->children.push_back(new_node);
    parser_AssociatedItem(new_node);
    lexer.expect({TokenType::Punctuation, "}"});
}

void Parser::parser_Implementation(AstNode *node) {
    node->type = AstNodetype::Implementation;
    lexer.expect({TokenType::Keyword, "impl"});

    node->value = lexer.expect(TokenType::Identifier).value;

    if (lexer.peek_next_token() == (Token){TokenType::Keyword, "for"}) {
        lexer.get_next_token();
        auto new_node = new AstNode;
        node->children.push_back(new_node);
        parser_Type(new_node);
    }

    lexer.expect({TokenType::Punctuation, "{"});
    auto new_node = new AstNode;
    node->children.push_back(new_node);
    parser_AssociatedItem(new_node);
    lexer.expect({TokenType::Punctuation, "}"});
}

void Parser::parser_AssociatedItem(AstNode *node) {
    node->type = AstNodetype::AssociatedItem;
    while (lexer.peek_next_token() != (Token){TokenType::Punctuation, "}"}) {
        auto token = lexer.peek_next_token();
        if (token.type != TokenType::Keyword) throw std::runtime_error("RE");
        if (token.value == "const") {
            auto new_node = new AstNode;
            node->children.push_back(new_node);
            parser_ConstantItem(new_node);
        }
        else if (token.value == "fn") {
            auto new_node = new AstNode;
            node->children.push_back(new_node);
            parser_Function(new_node);
        }
        else throw std::runtime_error("RE");
    }
}

void Parser::parser_FunctionParameters(AstNode *node) {
    node->type = AstNodetype::FunctionParameters;

    auto token = lexer.peek_next_token();

    bool is_comma = true, first = true;
    while (token != (Token){TokenType::Punctuation, ")"}) {
        if (is_comma == false) throw std::runtime_error("RE");
        is_comma = false;

        auto new_node = new AstNode;
        parser_TypedIdentifier(new_node, first); first = false;
        node->children.push_back(new_node);

        token = lexer.peek_next_token();
        if (token == (Token){TokenType::Punctuation, ","}) {
            lexer.get_next_token();
            is_comma = true;
            token = lexer.peek_next_token();
        }
    }
}

void Parser::parser_TypedIdentifier(AstNode *node, bool can_self) {
    node->type = AstNodetype::TypedIdentifier;

    auto token = lexer.peek_next_token();
    if (token == (Token){TokenType::Operator, "&"}) {
        lexer.get_next_token(); token = lexer.peek_next_token();
        auto new_node = new AstNode;
        new_node->type = AstNodetype::Quantifier;
        new_node->value = "&";
        node->children.push_back(new_node);
    }

    if (token == (Token){TokenType::Keyword, "mut"}) {
        lexer.get_next_token(); token = lexer.peek_next_token();
        auto new_node = new AstNode;
        new_node->type = AstNodetype::Quantifier;
        new_node->value = "mut";
        node->children.push_back(new_node);
    }

    if (token == (Token){TokenType::Keyword, "self"}) {
        if (can_self == false) throw std::runtime_error("RE");
        lexer.get_next_token(); token = lexer.peek_next_token();
        if (!node->children.empty() && token == (Token){TokenType::Punctuation, ":"})
            throw std::runtime_error("RE");
        node->type = AstNodetype::Self;
    }
    else {
        node->value = lexer.expect(TokenType::Identifier).value;
        lexer.expect((Token){TokenType::Punctuation, ":"});

        auto new_node = new AstNode;
        parser_Type(new_node);
        node->children.push_back(new_node);
    }
}

void Parser::parser_StructParameters(AstNode *node) {
    node->type = AstNodetype::StructParameters;
    auto token = lexer.peek_next_token();

    bool is_comma = true;
    while (token != (Token){TokenType::Punctuation, "}"}) {
        if (is_comma == false) throw std::runtime_error("RE");
        is_comma = false;

        auto new_node = new AstNode;
        parser_TypedIdentifier(new_node, false);
        node->children.push_back(new_node);

        token = lexer.peek_next_token();
        if (token == (Token){TokenType::Punctuation, ","}) {
            lexer.get_next_token();
            is_comma = true;
            token = lexer.peek_next_token();
        }
    }
}

void Parser::parser_Type(AstNode *node) {
    auto token = lexer.peek_next_token();

    //reference type
    if (token == (Token){TokenType::Operator, "&"} || token == (Token){TokenType::Operator, "&&"}) {
        if (token == (Token){TokenType::Operator, "&&"}) {
            auto new_node = new AstNode;
            node->children.push_back(new_node);
            node->type = AstNodetype::Unary_Operator;
            node->value = "&";
            node = new_node;
        }

        lexer.get_next_token();
        auto new_node = new AstNode;
        node->type = AstNodetype::Unary_Operator;
        node->children.push_back(new_node);

        token = lexer.peek_next_token();
        if (token == (Token){TokenType::Keyword, "mut"}) {
            lexer.get_next_token();
            node->value = "&mut";
        }
        else node->value = "&";

        node = new_node;
        token = lexer.peek_next_token();
    }

    node->type = AstNodetype::Type;

    //unit type
    if (token == (Token){TokenType::Punctuation, "("}) {
        lexer.get_next_token();
        lexer.expect({TokenType::Punctuation, ")"});
        node->value = "()";
        return ;
    }

    //array type
    if (token == (Token){TokenType::Punctuation, "["}) {
        lexer.get_next_token();
        auto new_node = new AstNode;
        parser_Type(new_node);
        node->children.push_back(new_node);

        lexer.expect({TokenType::Punctuation, ";"});

        new_node = new AstNode;
        parser_Expression(new_node);
        node->children.push_back(new_node);

        lexer.expect({TokenType::Punctuation, "]"});
        return ;
    }

    //type path
    if (token.type == TokenType::Identifier) {
        node->value = lexer.expect(TokenType::Identifier).value;
    }
    else if (token == (Token){TokenType::Keyword, "Self"}) {
        lexer.get_next_token();
        auto new_node = new AstNode;
        new_node->type = AstNodetype::Self;
        node->children.push_back(new_node);
    }
    else throw std::runtime_error("RE");
}

void Parser::parser_Statements(AstNode *node) {
    node->type = AstNodetype::Statements;

    auto token = lexer.peek_next_token();
    while (token != (Token){TokenType::Punctuation, "}"}) {
        if (token == (Token){TokenType::Punctuation, ";"}) {
            lexer.get_next_token();
        }
        else if (token.type == TokenType::Keyword && (token.value == "fn" || token.value == "struct" || token.value == "enum" || token.value == "const" || token.value == "trait" || token.value == "impl")) {
            parser_Item(node);
        }
        else if (token == (Token){TokenType::Keyword, "let"}) {
            auto new_node = new AstNode;
            parser_LetStatement(new_node);
            node->children.push_back(new_node);
        }
        else {
            auto new_node = new AstNode;
            parser_Expression_checkRes(new_node);
            node->children.push_back(new_node);
        }

        token = lexer.peek_next_token();
    }
}

void Parser::parser_LetStatement(AstNode *node) {
    node->type = AstNodetype::LetStatement;

    lexer.expect((Token){TokenType::Keyword, "let"});
    auto new_node = new AstNode;
    new_node->type = AstNodetype::Pattern;
    node->children.push_back(new_node);

    if (lexer.peek_next_token() == (Token){TokenType::Keyword, "mut"}) {
        new_node->value = "mut";
        lexer.get_next_token();
    }
    else {
        new_node->value = "const";
    }

    node->value = lexer.expect(TokenType::Identifier).value;

    new_node = new AstNode;
    node->children.push_back(new_node);
    if (lexer.peek_next_token() == (Token){TokenType::Punctuation, ":"}) {
        lexer.get_next_token();
        parser_Type(new_node);
    }
    else {
        new_node->type = AstNodetype::Type;
        new_node->value = "_";
    }

    lexer.expect((Token){TokenType::Operator, "="});
    new_node = new AstNode;
    node->children.push_back(new_node);
    parser_Expression(new_node);
    lexer.expect((Token){TokenType::Punctuation, ";"});
}

void Parser::parser_Expression(AstNode *node, bool only_flag) {
    auto tmp = lexer.peek_next_token().value;
    node->type = AstNodetype::Expression;
    node->children.push_back(pratt_Expression(0, only_flag));
}

void Parser::parser_Expression_checkRes(AstNode *node) {
    node->type = AstNodetype::Expression;
    parser_Expression(node, true);
    if (lexer.peek_next_token() != (Token){TokenType::Punctuation, ";"}) node->type = AstNodetype::Return_Cur;
        else lexer.get_next_token();
}

void Parser::parser_ArrayElements(AstNode *node) {
    node->type = AstNodetype::ArrayElements;
    int is_semicolon = -1;//-1:unkown, 0:only ,   , 1: one semicolon!
    //-1:unknown, 0: [a, b], 1: [a: b]
    auto new_group = new AstNode;
    bool is_seperator = true;
    while (lexer.peek_next_token() != (Token){TokenType::Punctuation, "]"}) {
        if (is_seperator == false) throw std::runtime_error("RE");
        auto now_node = new AstNode;
        parser_Expression(now_node);
        new_group->children.push_back(now_node);
        auto next_token = lexer.peek_next_token();
        if (next_token == (Token){TokenType::Punctuation, ","}) {
            lexer.get_next_token();
            if (is_semicolon == 1) throw std::runtime_error("RE");
            is_semicolon = 0;
            is_seperator = true;
            continue;
        }
        if (next_token == (Token){TokenType::Punctuation, ";"}) {
            lexer.get_next_token();
            if (is_semicolon == 0 || is_semicolon == 1) throw std::runtime_error("RE");
            is_semicolon = 1;
            is_seperator = true;
            continue;
        }
        is_seperator = false;
    }

    if (is_semicolon == -1) is_semicolon = 0;
    if (is_semicolon == 0) {
        node->children.push_back(new_group);
        new_group->type = AstNodetype::GroupExpression;
    }
    else {//is_semicolon == 1
        if (is_seperator == true) throw std::runtime_error("RE");
        if (new_group->children.size() != 2) throw std::runtime_error("RE");
        node->children = new_group->children;//then we don't need new_group node anymore
    }
}

bool check_prefix_value(const Token& token) {
    if (token == (Token){TokenType::Operator, "-"}) return true;
    if (token == (Token){TokenType::Operator, "!"}) return true;
    if (token == (Token){TokenType::Operator, "*"}) return true;
    if (token == (Token){TokenType::Operator, "&"}) return true;
    if (token == (Token){TokenType::Keyword, "return"}) return true;
    if (token == (Token){TokenType::Keyword, "break"}) return true;
    return false;
}

bool check_Item(const Token& token) {
    if (token.type == TokenType::CharLiteral) return true;
    if (token.type == TokenType::FloatLiteral) return true;
    if (token.type == TokenType::IntegerLiteral) return true;
    if (token.type == TokenType::Identifier) return true;
    if (token.type == TokenType::StringLiteral) return true;
    if (token == (Token){TokenType::Punctuation, "{"}) return true;
    if (token == (Token){TokenType::Punctuation, "["}) return true;
    if (token == (Token){TokenType::Punctuation, "("}) return true;
    if (token == (Token){TokenType::Keyword, "true"}) return true;
    if (token == (Token){TokenType::Keyword, "false"}) return true;
    if (token == (Token){TokenType::Keyword, "self"}) return true;
    if (token == (Token){TokenType::Keyword, "Self"}) return true;
    if (token == (Token){TokenType::Keyword, "break"}) return true;
    if (token == (Token){TokenType::Keyword, "continue"}) return true;
    if (token == (Token){TokenType::Keyword, "return"}) return true;
    if (token == (Token){TokenType::Keyword, "loop"}) return true;
    if (token == (Token){TokenType::Keyword, "while"}) return true;
    if (token == (Token){TokenType::Keyword, "if"}) return true;
    if (token == (Token){TokenType::Keyword, "else"}) return true;
    return false;
}

bool check_infix_value(const Token& token) {
    if (token == (Token){TokenType::Operator, "+"}) return true;
    if (token == (Token){TokenType::Operator, "-"}) return true;
    if (token == (Token){TokenType::Operator, "*"}) return true;
    if (token == (Token){TokenType::Operator, "/"}) return true;
    if (token == (Token){TokenType::Operator, "%"}) return true;
    if (token == (Token){TokenType::Operator, "<<"}) return true;
    if (token == (Token){TokenType::Operator, ">>"}) return true;
    if (token == (Token){TokenType::Operator, "<"}) return true;
    if (token == (Token){TokenType::Operator, ">"}) return true;
    if (token == (Token){TokenType::Operator, "<="}) return true;
    if (token == (Token){TokenType::Operator, ">="}) return true;
    if (token == (Token){TokenType::Operator, "=="}) return true;
    if (token == (Token){TokenType::Operator, "!="}) return true;
    if (token == (Token){TokenType::Operator, "&&"}) return true;
    if (token == (Token){TokenType::Operator, "||"}) return true;
    if (token == (Token){TokenType::Operator, "&"}) return true;
    if (token == (Token){TokenType::Operator, "|"}) return true;
    if (token == (Token){TokenType::Operator, "^"}) return true;
    if (token == (Token){TokenType::Operator, "="}) return true;
    if (token == (Token){TokenType::Operator, "+="}) return true;
    if (token == (Token){TokenType::Operator, "-="}) return true;
    if (token == (Token){TokenType::Operator, "*="}) return true;
    if (token == (Token){TokenType::Operator, "/="}) return true;
    if (token == (Token){TokenType::Operator, "%="}) return true;
    if (token == (Token){TokenType::Operator, "<<="}) return true;
    if (token == (Token){TokenType::Operator, ">>="}) return true;
    if (token == (Token){TokenType::Operator, "&="}) return true;
    if (token == (Token){TokenType::Operator, "|="}) return true;
    if (token == (Token){TokenType::Operator, "^="}) return true;
    if (token == (Token){TokenType::Operator, "::"}) return true;
    if (token == (Token){TokenType::Punctuation, "."}) return true;
    if (token == (Token){TokenType::Punctuation, "("}) return true;
    if (token == (Token){TokenType::Punctuation, "["}) return true;
    if (token == (Token){TokenType::Punctuation, "{"}) return true;
    if (token == (Token){TokenType::Keyword, "as"}) return true;
    return false;

}

AstNode* get_prefix(const Token& token) {
    auto node = new AstNode;
    if (token.type == TokenType::Identifier && (token.value == "i32" || token.value == "u32" || token.value == "isize" || token.value == "usize" || token.value == "char" || token.value == "str" ||token.value == "String" || token.value == "bool")) {
        node->type = AstNodetype::Type;
        node->value = token.value;
    }
    else if (token.type == TokenType::Identifier) {
        node->type = AstNodetype::Identifier;
        node->value = token.value;
    }
    else if (token.type == TokenType::CharLiteral) {
        node->type = AstNodetype::Char_Literal;
        node->value = token.value;
    }
    else if (token.type == TokenType::FloatLiteral) {
        node->type = AstNodetype::Float_Literal;
        node->value = token.value;
    }
    else if (token.type == TokenType::IntegerLiteral) {
        node->type = AstNodetype::Integer_Literal;
        node->value = token.value;
    }
    else if (token.type == TokenType::StringLiteral) {
        node->type = AstNodetype::String_Literal;
        node->value = token.value;
    }
    else if (token.type == TokenType::Keyword && (token.value == "false" || token.value == "true")) {
        node->type = AstNodetype::Bool_Literal;
        node->value = token.value;
    }
    else if (token == (Token){TokenType::Keyword, "self"}) {
        node->type = AstNodetype::Self;
    }
    else if (token == (Token){TokenType::Keyword, "Self"}) {
        node->type = AstNodetype::Type;
        node->value = token.value;
    }
    else if (token.type == TokenType::Keyword && (token.value == "bool" || token.value == "i32" || token.value == "u32" || token.value == "isize" || token.value == "usize" || token.value == "char" || token.value == "str" || token.value == "String")) {
        node->type = AstNodetype::Type;
        node->value = token.value;
    }
    else if (token == (Token){TokenType::Punctuation, "("}) {
        node->type = AstNodetype::Type;
        node->value = "()";
    }
    else if (token == (Token){TokenType::Keyword, "break"}) {
        node->type = AstNodetype::Break;
    }
    else if (token == (Token){TokenType::Keyword, "continue"}) {
        node->type = AstNodetype::Continue;
    }
    else if (token == (Token){TokenType::Keyword, "return"}) {
        node->type = AstNodetype::Return;
    }
    else if (token.type == TokenType::Operator) {
        node->type = AstNodetype::Unary_Operator;
        node->value = token.value;
    }
    return node;
}

AstNode* get_infix(const Token& token) {
    auto node = new AstNode;
    if (token.type == TokenType::Operator || token.type == TokenType::Punctuation) {
        node->type = AstNodetype::Binary_Operator;
        node->value = token.value;
    }
    else if (token == (Token){TokenType::Keyword, "as"}) {
        node->type = AstNodetype::Binary_Operator;
        node->value = token.value;
    }
    return node;
}

int get_prefix_precedence(const Token& token) {
    if (token.type == TokenType::Operator && (token.value == "-" || token.value == "!" || token.value == "&" || token.value == "*" || token.value == "/")) {
        return 120;
    }
    if (token.type == TokenType::Keyword && (token.value == "break" || token.value == "return")) {
        return 0;
    }
    return -1000;//unknown
}

int get_infix_precedence(const Token& token) {
    if (token.type == TokenType::Operator && (token.value == "=" || token.value == "+=" || token.value == "-=" || token.value == "*=" || token.value == "/=" || token.value == "%=" || token.value == "|=" || token.value == "&=" || token.value == "^=" || token.value == "<<=" || token.value == ">>=")) {
        return 10;
    }
    if (token == (Token){TokenType::Operator, "||"}) {
        return 20;
    }
    if (token == (Token){TokenType::Operator, "&&"}) {
        return 30;
    }
    if (token.type == TokenType::Operator && (token.value == "==" || token.value == "!=" || token.value == "<=" || token.value == ">=" || token.value == "<" || token.value == ">")) {
        return 40;
    }
    if (token == (Token){TokenType::Operator, "|"}) {
        return 50;
    }
    if (token == (Token){TokenType::Operator, "^"}) {
        return 60;
    }
    if (token == (Token){TokenType::Operator, "&"}) {
        return 70;
    }
    if (token == (Token){TokenType::Operator, "<<"} || token == (Token){TokenType::Operator, ">>"}) {
        return 80;
    }
    if (token == (Token){TokenType::Operator, "+"} || token == (Token){TokenType::Operator, "-"}) {
        return 90;
    }
    if (token == (Token){TokenType::Operator, "*"} || token == (Token){TokenType::Operator, "/"} || token == (Token){TokenType::Operator, "%"}) {
        return 100;
    }
    if (token == (Token){TokenType::Keyword, "as"}) {
        return 110;
    }
    if (token == (Token){TokenType::Punctuation, "("} || token == (Token){TokenType::Punctuation, "["} || token == (Token){TokenType::Punctuation, "{"}) {
        return 130;
    }
    if (token == (Token){TokenType::Punctuation, "."}) {
        return 140;
    }
    if (token == (Token){TokenType::Operator, "::"}) {
        return 150;
    }
    return -1000;
}

AstNode* Parser::pratt_Expression(int precedence, bool only_flag = false) {
    AstNode *root = nullptr, *node = nullptr;

    while (true) {
        auto prefix_token = lexer.get_next_token();
        if (prefix_token == (Token){TokenType::Punctuation, "("}) {
            if (lexer.peek_next_token() == (Token){TokenType::Punctuation, ")"}) {
                lexer.get_next_token();
            }
            else {
                auto new_node = pratt_Expression(0);
                lexer.expect((Token){TokenType::Punctuation, ")"});
                if (node != nullptr) node->children.push_back(new_node);
                else root = new_node;
                break;
            }
        }
        else if (prefix_token == (Token){TokenType::Punctuation, "["}) {
            auto new_node = new AstNode;
            parser_ArrayElements(new_node);
            lexer.expect((Token){TokenType::Punctuation, "]"});
            if (node != nullptr) node->children.push_back(new_node);
            else root = new_node;
            break;
        }
        else if (prefix_token == (Token){TokenType::Punctuation, "{"}) {
            auto new_node = new AstNode;
            parser_Statements(new_node);
            lexer.expect((Token){TokenType::Punctuation, "}"});
            if (node != nullptr) node->children.push_back(new_node);
            else root = new_node;
            if (only_flag) return root;
            break;
        }
        else if (prefix_token == (Token){TokenType::Keyword, "loop"}) {
            lexer.expect((Token){TokenType::Punctuation, "{"});
            auto new_node = new AstNode;
            new_node->type = AstNodetype::Loop;
            if (node != nullptr) node->children.push_back(new_node);
            else root = new_node;
            auto block_node = new AstNode;
            parser_Statements(block_node);
            new_node->children.push_back(block_node);
            lexer.expect((Token){TokenType::Punctuation, "}"});
            if (only_flag) return root;
            break;
        }
        else if (prefix_token == (Token){TokenType::Keyword, "while"}) {
            auto new_node = new AstNode;
            new_node->type = AstNodetype::While;
            if (node != nullptr) node->children.push_back(new_node);
            else root = new_node;
            auto block_node = new AstNode;
            new_node->children.push_back(block_node);
            lexer.expect((Token){TokenType::Punctuation, "("});
            parser_Expression(block_node);
            lexer.expect((Token){TokenType::Punctuation, ")"});

            block_node = new AstNode;
            new_node->children.push_back(block_node);
            lexer.expect((Token){TokenType::Punctuation, "{"});
            parser_Statements(block_node);
            lexer.expect((Token){TokenType::Punctuation, "}"});

            if (only_flag) return root;
            break;
        }
        else if (prefix_token == (Token){TokenType::Keyword, "if"}) {
            auto new_node = new AstNode;
            new_node->type = AstNodetype::If;
            if (node != nullptr) node->children.push_back(new_node);
            else root = new_node;
            node = new_node;

            auto block_node = new AstNode;
            new_node->children.push_back(block_node);
            lexer.expect((Token){TokenType::Punctuation, "("});
            parser_Expression(block_node);
            lexer.expect((Token){TokenType::Punctuation, ")"});

            block_node = new AstNode;
            new_node->children.push_back(block_node);
            lexer.expect((Token){TokenType::Punctuation, "{"});
            parser_Statements(block_node);
            lexer.expect((Token){TokenType::Punctuation, "}"});
            if (lexer.peek_next_token() != (Token){TokenType::Keyword, "else"}) {
                return root;
                // if (only_flag) return root;
                // break;
            }
            continue;
        }
        else if (prefix_token == (Token){TokenType::Keyword, "else"}) {
            auto new_node = new AstNode;
            new_node->type = AstNodetype::Else;
            if (node != nullptr) node->children.push_back(new_node);
            else root = new_node;
            node = new_node;

            if (lexer.peek_next_token() == (Token){TokenType::Keyword, "if"}) {//if expression
                continue;
            }

            //block expression
            auto block_node = new AstNode;
            new_node->children.push_back(block_node);
            lexer.expect((Token){TokenType::Punctuation, "{"});
            parser_Statements(block_node);
            lexer.expect((Token){TokenType::Punctuation, "}"});

            return root;
            // if (only_flag) return root;
            // break;
        }
        else if (prefix_token == (Token){TokenType::Operator, "&"} || prefix_token == (Token){TokenType::Operator, "&&"}) {
            if (prefix_token == (Token){TokenType::Operator, "&&"}) {
                auto new_node = new AstNode;
                new_node->type = AstNodetype::Unary_Operator;
                new_node->value = "&";
                if (node != nullptr) node->children.push_back(new_node);
                else root = new_node;
                node = new_node;
            }
            auto new_node = new AstNode;
            new_node->type = AstNodetype::Unary_Operator;
            new_node->value = "&";
            if (lexer.peek_next_token() == (Token){TokenType::Keyword, "mut"}) {
                lexer.get_next_token();
                new_node->value = "&mut";
            }
            if (node != nullptr) node->children.push_back(new_node);
            else root = new_node;
            node = new_node;

            node->children.push_back(pratt_Expression(get_prefix_precedence((Token){TokenType::Operator, "&"})));
            break;
        }
        auto new_node = get_prefix(prefix_token);
        if (node != nullptr) node->children.push_back(new_node);
        else root = new_node;
        node = new_node;
        if (check_prefix_value(prefix_token)) {//bie ji
            if ((prefix_token != (Token){TokenType::Keyword, "return"} && prefix_token != (Token){TokenType::Keyword, "break"}) || check_prefix_value(lexer.peek_next_token()) || check_Item(lexer.peek_next_token())) {
                node->children.push_back(pratt_Expression(get_prefix_precedence(prefix_token)));
            }
            break;
        }
        if (check_Item(prefix_token)) {//go to infix
            if (prefix_token == (Token){TokenType::Keyword, "continue"} && (check_prefix_value(lexer.peek_next_token()) || check_Item(lexer.peek_next_token())))
                throw std::runtime_error("RE");
            break;
        }
        throw std::runtime_error("RE");
    }

    while (true) {
        auto infix_token = lexer.peek_next_token();
        if (check_infix_value(infix_token) == false) break;
        auto now_precedence = get_infix_precedence(infix_token);
        if (now_precedence <= precedence) break;

        if (infix_token == (Token){TokenType::Punctuation, "("}) {
            lexer.get_next_token();
            auto new_node = new AstNode;
            new_node->type = AstNodetype::FunctionCall;
            new_node->children.push_back(root);
            root = new_node;
            new_node = new AstNode;
            root->children.push_back(new_node);
            parser_CallParams(new_node);
            lexer.expect((Token){TokenType::Punctuation, ")"});
            continue;
        }
        else if (infix_token == (Token){TokenType::Punctuation, "["}) {
            lexer.get_next_token();
            auto new_node = new AstNode;
            new_node->type = AstNodetype::ArrayIndex;
            new_node->children.push_back(root);
            root = new_node;
            new_node = new AstNode;
            root->children.push_back(new_node);
            parser_Expression(new_node);
            lexer.expect((Token){TokenType::Punctuation, "]"});
            continue;
        }
        else if (infix_token == (Token){TokenType::Punctuation, "{"}) {
            lexer.get_next_token();
            auto new_node = new AstNode;
            new_node->type = AstNodetype::StructField;
            new_node->children.push_back(root);
            root = new_node;
            new_node = new AstNode;
            new_node->type = AstNodetype::Fields;
            root->children.push_back(new_node);

            bool is_separator = true;
            while (lexer.peek_next_token() != (Token){TokenType::Punctuation, "}"}) {
                if (is_separator == false) throw std::runtime_error("RE");
                is_separator = false;
                auto new_field = new AstNode;
                new_field->type = AstNodetype::Field;
                new_node->children.push_back(new_field);
                new_field->value = lexer.expect(TokenType::Identifier).value;

                lexer.expect((Token){TokenType::Punctuation, ":"});
                auto new_expr = new AstNode;
                new_field->children.push_back(new_expr);
                parser_Expression(new_expr);

                if (lexer.peek_next_token() == (Token){TokenType::Punctuation, ","}) {
                    is_separator = true; lexer.get_next_token();
                }
            }
            lexer.expect((Token){TokenType::Punctuation, "}"});
            continue;
        }

        auto new_node = get_infix(infix_token);
        lexer.get_next_token();
        new_node->children.push_back(root);
        root = new_node;
        if (now_precedence == 10) now_precedence = 5;
        root->children.push_back(pratt_Expression(now_precedence));
    }
    return root;
}

void Parser::parser_CallParams(AstNode *node) {
    node->type = AstNodetype::CallParams;
    bool is_seperator = true;
    while (lexer.peek_next_token() != (Token){TokenType::Punctuation, ")"}) {
        if (is_seperator == false) throw std::runtime_error("RE");
        is_seperator = false;
        auto new_node = new AstNode;
        node->children.push_back(new_node);
        parser_Expression(new_node);
        if (lexer.peek_next_token() == (Token){TokenType::Punctuation, ","}) {
            is_seperator = true; lexer.get_next_token();
        }
    }
}