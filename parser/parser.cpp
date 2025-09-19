//
// Created by Sakura on 25-9-17.
//

#include "parser.h"
#include "../lexer.h"
#include <memory>
#include<optional>
#include <utility>

#include "../AST/node/expression.h"
#include "../AST/node/Literal_Type.h"
#include "../AST/node/parser_type.h"
#include "../AST/node/patterns.h"

Parser::Parser():token_pos(0){}
Parser::Parser(std::vector<Token> tokens) : tokens(move(tokens)), token_pos(0) {}

struct ShortandSelf {
    bool have_ref;
    bool is_mut;

    ShortandSelf() : have_ref(false), is_mut(false) {}
};

struct TypedSelf {
    bool is_mut;
    std::shared_ptr<TypeNode> type;

    TypedSelf() : is_mut(false) {}
    ~TypedSelf() = default;
};

struct SelfParam {
    bool is_shortandself;
    std::optional<ShortandSelf> shortandself;
    std::optional<TypedSelf> typedself;

    SelfParam() : is_shortandself(false), typedself() {}
};

struct FunctionParam {
    std::shared_ptr<Pattern> pattern;
    std::shared_ptr<TypeNode> type;

    FunctionParam(std::shared_ptr<Pattern> pattern, std::shared_ptr<TypeNode> type) : pattern(move(pattern)), type(move(type)) {}
};

struct FunctionParameters {
    std::optional<SelfParam> selfparam;
    std::vector<FunctionParam> params;
};

class FunctionItem : public BasicNode {
public:
    std::string Identifier;
    std::optional<FunctionParameters> function_parameters;
    std::optional<std::shared_ptr<TypeNode>> return_type;
    std::optional<BlockExpression> block_expression;

    FunctionItem(std::string Identifier, std::optional<FunctionParameters> function_parameters, std::optional<std::shared_ptr<TypeNode>> return_type, std::optional<BlockExpression> block_expression) : Identifier(std::move(Identifier)), function_parameters(std::move(function_parameters)), return_type(std::move(return_type)), block_expression(std::move(block_expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

std::optional<ShortandSelf> parser_Shortandself(const std::vector<Token>& tokens, int &pos) {
    if (pos >= tokens.size()) return std::nullopt;
    auto now_token = tokens[pos];
    ShortandSelf shortandself;
    if (now_token.type == TokenType::Operator && now_token.value == "&") {
        shortandself.have_ref = true;
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
    }
    if (now_token.type == TokenType::Keyword && now_token.value == "mut") {
        shortandself.is_mut = true;
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
    }
    if (now_token.type == TokenType::Keyword && now_token.value == "self") {
        pos++;
        return shortandself;
    }
    return std::nullopt;
}

std::optional<PathIdentSegment> parser_PathIdentSegment(const std::vector<Token>& tokens, int &pos) {
    if (pos >= tokens.size()) return std::nullopt;
    const auto& now_token = tokens[pos];
    if (now_token.type == TokenType::Keyword && now_token.value == "self") {
        pos++;
        return PathIdentSegment(true, false, "");
    }
    if (now_token.type == TokenType::Keyword && now_token.value == "Self") {
        pos++;
        return PathIdentSegment(false, true, "");
    }
    if (now_token.type == TokenType::Identifier) {
        pos++;
        return PathIdentSegment(false, false, now_token.value);
    }
    return std::nullopt;
}

//Start Expression Parser

std::optional<std::shared_ptr<Expression>> parser_Expression(const std::vector<Token>& tokens, int &pos) {
    if (pos >= tokens.size()) return std::nullopt;
    
}

//End Expression Parser

std::optional<std::shared_ptr<TypeNode>> parser_Typenode(const std::vector<Token>& tokens, int &pos) {
    if (pos >= tokens.size()) return std::nullopt;
    auto now_token = tokens[pos];
    if (now_token.type == TokenType::Punctuation && now_token.value == "(") {//UnitType
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
        if (now_token.type == TokenType::Punctuation && now_token.value == ")") {
            pos++;
            std::shared_ptr<TypeNode> unit_type = std::make_shared<UnitType>();
            return unit_type;
        }
        return std::nullopt;
    }
    if (now_token.type == TokenType::Punctuation && now_token.value == "[") {//ArrayType
        pos++;
        std::optional<std::shared_ptr<TypeNode>> return_type = parser_Typenode(tokens, pos);
        if (return_type == std::nullopt) return std::nullopt;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
        if (now_token.type == TokenType::Punctuation && now_token.value == ";") {
            pos++;
            std::optional<std::shared_ptr<Expression>> expression = parser_Expression(tokens, pos);
            if (expression == std::nullopt) return std::nullopt;

            if (pos >= tokens.size()) return std::nullopt;
            now_token = tokens[pos];
            if (now_token.type == TokenType::Punctuation && now_token.value == "]") {
                pos++;
                return std::make_shared<ArrayType>(return_type.value(), expression.value());
            }
        }
        return std::nullopt;//not done parser_expression yet!!!!!!!!!!!!
    }
    if (now_token.type == TokenType::Operator && now_token.value == "&") {//ReferenceType
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
        bool is_mut = false;
        if (now_token.type == TokenType::Keyword && now_token.value == "mut") {
            is_mut = true;
            pos++;
            if (pos >= tokens.size()) return std::nullopt;
        }
        std::optional<std::shared_ptr<TypeNode>> return_type = parser_Typenode(tokens, pos);
        if (return_type == std::nullopt) return std::nullopt;
        return std::make_shared<ReferenceType>(is_mut, return_type.value());
    }
    //TypePath
    std::optional<PathIdentSegment> segment = parser_PathIdentSegment(tokens, pos);
    if (segment == std::nullopt) return std::nullopt;
    if (pos < tokens.size()) {
        now_token = tokens[pos];
        if (now_token.type == TokenType::Operator && now_token.value == "::") {
            pos++;
            std::optional<PathIdentSegment> true_segment = parser_PathIdentSegment(tokens, pos);
            if (true_segment == std::nullopt) return std::nullopt;
            return std::make_shared<TypePath>(true_segment.value(), true, segment.value());
        }
    }
    return std::make_shared<TypePath>(segment.value(), false, PathIdentSegment());
}

std::optional<TypedSelf> parser_Typedself(const std::vector<Token>& tokens, int &pos) {
    if (pos >= tokens.size()) return std::nullopt;
    auto now_token = tokens[pos];
    TypedSelf typedself;
    if (now_token.type == TokenType::Keyword && now_token.value == "mut") {
        typedself.is_mut = true;
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
    }
    if (now_token.type == TokenType::Keyword && now_token.value == "self") {
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
        if (now_token.type == TokenType::Punctuation && now_token.value == ":") {
            pos++;
            std::optional<std::shared_ptr<TypeNode>> literal_type = parser_Typenode(tokens, pos);
            if (literal_type == std::nullopt) return std::nullopt;
            typedself.type = literal_type.value();
            return typedself;
        }
    }
    return std::nullopt;
}


std::optional<SelfParam> parser_SelfParam(const std::vector<Token>& tokens, int &pos) {
    SelfParam selfparam;
    int shortandself_pos = pos;
    selfparam.shortandself = parser_Shortandself(tokens, shortandself_pos);
    int typedself_pos = pos;
    selfparam.typedself = parser_Typedself(tokens, typedself_pos);
    if (selfparam.typedself == std::nullopt && selfparam.shortandself == std::nullopt) return std::nullopt;
    if (selfparam.typedself != std::nullopt && selfparam.shortandself != std::nullopt) {
        if (shortandself_pos < typedself_pos) selfparam.shortandself = std::nullopt;
            else selfparam.typedself = std::nullopt;
    }
    pos = std::max(shortandself_pos, typedself_pos);
    if (selfparam.shortandself != std::nullopt) selfparam.is_shortandself = true;
    return selfparam;
}

std::optional<std::shared_ptr<Pattern>> parser_Pattern(const std::vector<Token>& tokens, int &pos) {
    if (pos >= tokens.size()) return std::nullopt;
    auto now_token = tokens[pos];
    if (now_token.type == TokenType::Operator && now_token.value == "_") {//WildcardPattern
        pos++;
        return std::make_shared<WildcardPattern>();
    }
    if (now_token.type == TokenType::Operator && (now_token.value == "&" || now_token.value == "&&")) {//ReferencePattern
        bool double_ref = now_token.value == "&&";
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
        if (now_token.type == TokenType::Keyword && now_token.value == "mut") {
            pos++;
            if (pos >= tokens.size()) return std::nullopt;
            auto pattern = parser_Pattern(tokens, pos);
            if (pattern == std::nullopt) return std::nullopt;
            return std::make_shared<ReferencePattern>(double_ref, true, pattern.value());
        }
        else {
            auto pattern = parser_Pattern(tokens, pos);
            if (pattern == std::nullopt) return std::nullopt;
            return std::make_shared<ReferencePattern>(double_ref, false, pattern.value());
        }
    }
    //IdentifierPattern
    bool have_ref = false, is_mut = false;
    if (now_token.type == TokenType::Keyword && now_token.value == "ref") {
        have_ref = true;
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
    }
    if (now_token.type == TokenType::Keyword && now_token.value == "mut") {
        is_mut = true;
        pos++;
        if (pos >= tokens.size()) return std::nullopt;
        now_token = tokens[pos];
    }
    if (now_token.type != TokenType::Identifier) return std::nullopt;
    return std::make_shared<IdentifierPattern>(have_ref, is_mut, now_token.value);
}

std::optional<FunctionParam> parser_FunctionParam(const std::vector<Token>& tokens, int &pos) {
    std::optional<std::shared_ptr<Pattern>> pattern = parser_Pattern(tokens, pos);
    if (pattern == std::nullopt) return std::nullopt;
    if (pos >= tokens.size()) return std::nullopt;
    auto now_token = tokens[pos];
    if (now_token.type != TokenType::Punctuation || now_token.value != ":") return std::nullopt;
    pos++;
    if (pos >= tokens.size()) return std::nullopt;
    now_token = tokens[pos];
    std::optional<std::shared_ptr<TypeNode>> type = parser_Typenode(tokens, pos);
    if (type == std::nullopt) return std::nullopt;
    return FunctionParam(pattern.value(), type.value());
}

void Parser::parser_program(std::shared_ptr<ProgramNode> root) {
    root->type = Program;

    while (token_pos < tokens.size()) {
        auto token = tokens[token_pos];

        if (token.type == TokenType::Keyword && token.value == "fn") {
            if (token_pos + 1 >= tokens.size()) throw;
            token_pos++;
            auto now_token = tokens[token_pos];
            if (now_token.type != TokenType::Identifier) throw ;
            std::string identifier = now_token.value;

            if (token_pos + 1 >= tokens.size()) throw;
            token_pos++;
            now_token = tokens[token_pos];
            if (now_token.type != TokenType::Punctuation && now_token.value != "(") throw ;

            std::optional<SelfParam> self_param;
            std::vector<FunctionParam> params;
            bool first = true;

            bool last_douhao = true;
            token_pos++;
            if (token_pos >= tokens.size()) throw;
            now_token = tokens[token_pos];
            while (now_token.type != TokenType::Punctuation || now_token.value != ")") {
                if (now_token.type == TokenType::Punctuation && now_token.value == ",") {
                    if (last_douhao) throw;
                    last_douhao = true;
                    token_pos++;
                    if (token_pos >= tokens.size()) throw;
                    now_token = tokens[token_pos];
                    continue;
                }
                if (last_douhao == false) throw;
                last_douhao = false;

                int selfparam_pos = token_pos;
                if (first) {
                    self_param = parser_SelfParam(tokens, selfparam_pos);
                    first = false;
                    if (self_param != std::nullopt) {
                        token_pos = selfparam_pos;
                        if (token_pos >= tokens.size()) throw;
                        now_token = tokens[token_pos];
                        continue;
                    }
                }
                auto function_param = parser_FunctionParam(tokens, token_pos);
                if (function_param == std::nullopt) throw;
                params.push_back(function_param.value());
                if (token_pos >= tokens.size()) throw;
                now_token = tokens[token_pos];
            }
            FunctionParameters function_params(self_param, params);

            //next is FunctionReturnType
            if (token_pos >= tokens.size()) throw;
            now_token = tokens[token_pos];
            std::optional<std::shared_ptr<TypeNode>> type = std::nullopt;
            if (now_token.type == TokenType::Operator || now_token.value == "->") {
                token_pos++;
                type = parser_Typenode(tokens, token_pos);
                if (type == std::nullopt) throw;
            }

            //next is BlockExpression
            if (token_pos >= tokens.size()) throw;
            now_token = tokens[token_pos];
            std::optional<BlockExpression> block = std::nullopt;
            if (now_token.type == TokenType::Punctuation && now_token.value == ";") {
                token_pos++;
            }
            else {
                block = parser_BlockExpression(tokens, token_pos);
                if (block == std::nullopt) throw;
            }

            auto function_item = std::make_shared<FunctionItem>(identifier, function_params, type, block);
            root->statements.push_back(function_item);
        }
    }
}
