//
// Created by Sakura on 25-8-27.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <utility>

#include "basic.h"
#include "Literal_Type.h"
#include <optional>

#include "basic.h"

class SemanticAnalyzer;

class Expression: BasicNode {
public:
    virtual ~Expression() = default;

    virtual void accept(SemanticAnalyzer &visitor) = 0;
};

class LiteralExpression final : Expression {
public:
    LiteralType type;
    std::string value;

    LiteralExpression(LiteralType type, std::string value): type(std::move(type)), value(std::move(value)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class PathExpression final : Expression {
public:
    struct Expression_Args {
        std::vector<std::shared_ptr<Expression>> arguments;
    };
    struct PathPattern {
        std::string identifier;
        std::optional<Expression_Args> parameter;
    };
    bool have_leading_colon;
    std::vector<PathPattern> paths;

    PathExpression(bool have_colon, std::vector<PathPattern> paths): have_leading_colon(have_colon), paths(std::move(paths)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class BlockExpression final : Expression {
public:
    std::vector<std::shared_ptr<BasicNode>> statement;
    std::optional<std::shared_ptr<Expression>> no_block_expression;

    BlockExpression(std::vector<std::shared_ptr<BasicNode>> statement, std::optional<std::shared_ptr<Expression>> no_block_expression = std::nullopt): statement(std::move(statement)), no_block_expression(std::move(no_block_expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

//Start Operator Expression

class BorrowExpression final : Expression {
public:
    bool two_borrow;//is true then two borrow, is false then one borrow
    bool is_mut;
    std::shared_ptr<Expression> expression;

    BorrowExpression(bool two_borrow, bool is_mut, std::shared_ptr<Expression> expression):two_borrow(two_borrow),is_mut(is_mut),expression(std::move(expression)){}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class DereferenceExpression final : Expression {
public:
    std::shared_ptr<Expression> expression;

    DereferenceExpression(std::shared_ptr<Expression> expression):expression(std::move(expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class NegateExpression final : Expression {
public:
    bool is_negation;//is true then negaion(-), is false then NOT(!)
    std::shared_ptr<Expression> expression;

    NegateExpression(bool is_negation, std::shared_ptr<Expression> expression):is_negation(is_negation), expression(std::move(expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class ArithmeticOrLogicalExpression final : Expression {
public:
    enum Arith_Symbol {
        Add,// +
        Sub,// -
        Mul,// *
        Div,// /
        Rem,// %
        BitAnd,// &
        BitOr,// |
        BitXor,// ^
        Shl,// <<
        Shr,// >>
    }symbol;
    std::shared_ptr<Expression> left_expression, right_expression;

    std::map<Arith_Symbol, std::string> arith_symbol_to_string = {
        {Add, "+"},
        {Sub, "-"},
        {Mul, "*"},
        {Div, "/"},
        {Rem, "%"},
        {BitAnd, "&"},
        {BitOr, "|"},
        {BitXor, "^"},
        {Shl, "<<"},
        {Shr, ">>"},
    };
    std::map<std::string, Arith_Symbol> string_to_arith_symbol = {
        {"+", Add},
        {"-", Sub},
        {"*", Mul},
        {"/", Div},
        {"%", Rem},
        {"&", BitAnd},
        {"|", BitOr},
        {"^", BitXor},
        {"<<", Shl},
        {">>", Shr},
    };

    ArithmeticOrLogicalExpression(std::shared_ptr<Expression> left_expression, Arith_Symbol symbol, std::shared_ptr<Expression> right_expression): left_expression(std::move(left_expression)), symbol(symbol), right_expression(std::move(right_expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class ComparisonExpression final : Expression {
public:
    enum Comp_Symbol {
        Eq,// ==
        Ne,// !=
        Gt,// >
        Lt,// <
        Ge,// >=
        Le,// <=
    }symbol;
    std::shared_ptr<Expression> left_expression, right_expression;

    std::map<Comp_Symbol, std::string> symbol_to_string = {
        {Eq, "=="},
        {Ne, "!="},
        {Gt, ">"},
        {Lt, "<"},
        {Ge, ">="},
        {Le, "<="},
    };
    std::map<std::string, Comp_Symbol> string_to_symbol = {
        {"==", Eq},
        {"!=", Ne},
        {">", Gt},
        {"<", Lt},
        {">=", Ge},
        {"<=", Le},
    };

    ComparisonExpression(std::shared_ptr<Expression> left_expression, Comp_Symbol symbol, std::shared_ptr<Expression> right_expression): left_expression(std::move(left_expression)), symbol(symbol), right_expression(std::move(right_expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class LazyBooleanExpression final : Expression {
public:
    bool is_logic_or;//is true means ||, is false means &&
    std::shared_ptr<Expression> left_expression, right_expression;

    LazyBooleanExpression(std::shared_ptr<Expression> left_expression, bool is_logic_or, std::shared_ptr<Expression> right_expression): left_expression(std::move(left_expression)), is_logic_or(is_logic_or), right_expression(std::move(right_expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class TypeCastExpression final : Expression {
public:
    std::shared_ptr<Expression> expression;
    LiteralType type;

    TypeCastExpression(std::shared_ptr<Expression> expression, LiteralType type): expression(std::move(expression)), type(std::move(type)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class AssignmentExpression final : Expression {
public:
    std::shared_ptr<Expression> left_expression, right_expression;

    AssignmentExpression(std::shared_ptr<Expression> left_expression, std::shared_ptr<Expression> right_expression): left_expression(std::move(left_expression)), right_expression(std::move(right_expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

class CompoundAssignmentExpression final : Expression {
public:
    enum Compound_Symbol {
        Add,// +=
        Sub,// -=
        Mul,// *=
        Div,// /=
        Rem,// %=
        BitAnd,// &=
        BitOr,// |=
        BitXor,// ^=
        Shl,// <<=
        Shr,// >>=
    }symbol;
    std::shared_ptr<Expression> left_expression, right_expression;

    std::map<Compound_Symbol, std::string> compound_symbol_to_string = {
        {Add, "+="},
        {Sub, "-="},
        {Mul, "*="},
        {Div, "/="},
        {Rem, "%="},
        {BitAnd, "&="},
        {BitOr, "|="},
        {BitXor, "^="},
        {Shl, "<<="},
        {Shr, ">>="},
    };
    std::map<std::string, Compound_Symbol> string_to_compound_symbol = {
        {"+=", Add},
        {"-=", Sub},
        {"*=", Mul},
        {"/=", Div},
        {"%=", Rem},
        {"&=", BitAnd},
        {"|=", BitOr},
        {"^=", BitXor},
        {"<<=", Shl},
        {">>=", Shr},
    };

    CompoundAssignmentExpression(std::shared_ptr<Expression> left_expression, Compound_Symbol symbol, std::shared_ptr<Expression> right_expression): left_expression(std::move(left_expression)), symbol(symbol), right_expression(std::move(right_expression)) {}

    void accept(SemanticAnalyzer &visitor) override {
        visitor.visit(*this);
    }
};

//End Operator Expression



#endif //EXPRESSION_H
