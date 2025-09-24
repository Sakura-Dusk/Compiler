#include "parser.h"

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
    if (!lexer.is_End()) throw;
}

void Parser::parser_Item(AstNode *node) {
    auto token = lexer.peek_next_token();
    if (token.type != TokenType::Keyword) throw;
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
    else throw;
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

    auto new_node = new AstNode;
    new_node->type = AstNodetype::Parameters;
    new_node->children.push_back(new_node);

    auto token = lexer.peek_next_token();
    if (token == (Token){TokenType::Punctuation, ";"}) {
        lexer.get_next_token();
    }
    else {
        lexer.expect({TokenType::Punctuation, "{"});
        parser_Parameters(new_node);
        lexer.expect({TokenType::Punctuation, "}"});
    }
}

void Parser::parser_Enumeration(AstNode *node) {
    node->type = AstNodetype::Enumeration;

    lexer.expect({TokenType::Keyword, "enum"});
    auto new_node = new AstNode;
    new_node->type = AstNodetype::Identifier;
    new_node->value = lexer.expect(TokenType::Identifier).value;
    node->children.push_back(new_node);

    auto node_items = new AstNode;
    node_items->type = AstNodetype::Enumeration_Items;
    node->children.push_back(node_items);
    lexer.expect({TokenType::Punctuation, "{"});

    bool is_comma = true;
    while (lexer.peek_next_token() != (Token){TokenType::Punctuation, "}"}) {
        if (is_comma == false) throw;
        is_comma = false;
        new_node = new AstNode;
        new_node->type = AstNodetype::Identifier;
        new_node->value = lexer.expect(TokenType::Identifier).value;
        node_items->children.push_back(new_node);

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

    auto new_node = new AstNode;
    bool is_Identifier = (lexer.peek_next_token().type == TokenType::Identifier);
    node->children.push_back(new_node);
    parser_Type(new_node);

    if (lexer.peek_next_token() == (Token){TokenType::Keyword, "for"}) {
        if (is_Identifier == false) throw;
        lexer.get_next_token();
        new_node = new AstNode;
        node->children.push_back(new_node);
        parser_Type(new_node);
    }

    lexer.expect({TokenType::Punctuation, "{"});
    new_node = new AstNode;
    node->children.push_back(new_node);
    parser_AssociatedItem(new_node);
    lexer.expect({TokenType::Punctuation, "}"});
}

void Parser::parser_AssociatedItem(AstNode *node) {
    node->type = AstNodetype::AssociatedItem;
    while (lexer.peek_next_token() != (Token){TokenType::Punctuation, "}"}) {
        auto token = lexer.peek_next_token();
        if (token.type != TokenType::Identifier) throw;
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
        else throw;
    }
}

void Parser::parser_FunctionParameters(AstNode *node) {
    node->type = AstNodetype::FunctionParameters;

    auto token = lexer.peek_next_token();
    bool amp = false, is_mut = false;
    if (token == (Token){TokenType::Operator, "&"}) {
        lexer.get_next_token();
        amp = true;
        token = lexer.peek_next_token();
    }
    if (token == (Token){TokenType::Keyword, "mut"}) {
        lexer.get_next_token();
        is_mut = true;
        token = lexer.peek_next_token();
    }

    bool is_comma = true;
    if (token == (Token){TokenType::Keyword, "self"}) {
        lexer.get_next_token();
        token = lexer.peek_next_token();

        auto new_node = new AstNode;
        new_node->type = AstNodetype::self;
        node->children.push_back(new_node);
        if (amp == true) {
            auto amp_node = new AstNode;
            amp_node->type = AstNodetype::Amp;
            new_node->children.push_back(amp_node);
        }
        if (is_mut == true) {
            auto mut_node = new AstNode;
            mut_node->type = AstNodetype::Mut;
            new_node->children.push_back(mut_node);
        }

        is_comma = false;
        if (token == (Token){TokenType::Punctuation, ","}) {
            lexer.get_next_token();
            is_comma = true;
            token = lexer.peek_next_token();
        }
    }

    is_mut = false;
    if (token == (Token){TokenType::Keyword, "mut"}) {
        lexer.get_next_token();
        is_mut = true;
        token = lexer.peek_next_token();
    }
    while (token.type == TokenType::Identifier) {
        if (is_comma == false) throw;
        is_comma = false;

        auto new_node = new AstNode;
        parser_TypedIdentifier(new_node, is_mut);
        node->children.push_back(new_node);

        token = lexer.peek_next_token();
        if (token == (Token){TokenType::Punctuation, ","}) {
            lexer.get_next_token();
            is_comma = true;
            token = lexer.peek_next_token();
        }

        is_mut = false;
        if (token == (Token){TokenType::Keyword, "mut"}) {
            lexer.get_next_token();
            is_mut = false;
            token = lexer.peek_next_token();
        }
    }
}

void Parser::parser_TypedIdentifier(AstNode *node, bool is_mut) {

}