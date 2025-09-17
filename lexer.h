//
// Created by Sakura on 25-8-17.
//

#ifndef LEXER_H
#define LEXER_H

#include <regex>
#include <map>

enum class TokenType {
    Keyword,
    Identifier,
    StringLiteral,
    CharLiteral,
    IntegerLiteral,
    FloatLiteral,
    Comment,
    Operator,
    Punctuation,
    Whitespace,
    Unknown
};

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Keyword: return "Keyword";
        case TokenType::Identifier: return "Identifier";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::CharLiteral: return "CharLiteral";
        case TokenType::IntegerLiteral: return "IntegerLiteral";
        case TokenType::FloatLiteral: return "FloatLiteral";
        case TokenType::Comment: return "Comment";
        case TokenType::Operator: return "Operator";
        case TokenType::Punctuation: return "Punctuation";
        case TokenType::Whitespace: return "Whitespace";
        case TokenType::Unknown: return "Unknown";
        default: return "UNKNOWN_TYPE";
    }
}

struct Token {
    TokenType type;
    std::string value;
    int number;
};

struct TokenPattern {
    TokenType type;
    std::regex regex;
    std::string patternString;
};


std::vector<TokenPattern> patterns;

void defineRustTokenPatterns() {
    patterns.push_back({TokenType::Comment, std::regex(R"(\/\/.*)"), "// Single-line comment"});
    patterns.push_back({TokenType::Comment, std::regex(R"(\/\*[\s\S]*?\*\/)"), "/* Block comment */"});

    patterns.push_back({TokenType::StringLiteral, std::regex(R"("(?:[^"\\]|\\.)*")"), "String literal"});
    patterns.push_back({TokenType::StringLiteral, std::regex(R"(r(#*)\"[\s\S]*?\"\1)"), "Raw string literal"});
    patterns.push_back({TokenType::StringLiteral, std::regex(R"(b"(?:[^"\\]|\\.)*")"), "Byte string literal"});
    patterns.push_back({TokenType::StringLiteral, std::regex(R"(br(#*)\"[\s\S]*?\"\1)"), "Raw byte string literal"});

    patterns.push_back({TokenType::CharLiteral, std::regex(R"(' (?:[^'\\]|\\.) ')"), "Character literal"});
    patterns.push_back({TokenType::CharLiteral, std::regex(R"(b' (?:[^'\\]|\\.) ')"), "Byte literal"});

    patterns.push_back({TokenType::FloatLiteral, std::regex(R"(\b\d+\.\d*(?:[eE][+-]?\d+)?(?:f32|f64)?\b)"), "Float literal"});
    patterns.push_back({TokenType::FloatLiteral, std::regex(R"(\b\.\d+(?:[eE][+-]?\d+)?(?:f32|f64)?\b)"), "Float literal (starting with dot)"});
    patterns.push_back({TokenType::FloatLiteral, std::regex(R"(\b\d+(?:[eE][+-]?\d+)(?:f32|f64)?\b)"), "Float literal (exp only)"});

    patterns.push_back({TokenType::IntegerLiteral, std::regex(R"(\b(?:0x[0-9a-fA-F_]+|0o[0-7_]+|0b[01_]+|\d[0-9_]*)(?:u8|u16|u32|u64|u128|usize|i8|i16|i32|i64|i128|isize)?\b)"), "Integer literal"});

    const std::vector<std::string> rustKeywords = {
        "as", "break", "const", "continue", "crate", "else", "enum", "extern", "false",//strict
        "fn", "for", "if", "in", "impl", "let", "loop", "match", "mod", "move", "mut",
        "pub", "ref", "return", "self", "Self", "static", "struct", "super", "trait", "true",
        "type", "unsafe", "use", "where", "while",

        "async", "await", "dyn",

        "abstract", "become", "box", "do", "final", "macro", "override",//reserved
        "priv", "typeof", "unsized", "virtual", "yield",
        "try",
        "gen",

        "'static", "macro_rules", "raw", "safe", "union",//weak
    };
    for (const auto& keyword : rustKeywords) {
        patterns.push_back({TokenType::Keyword, std::regex(R"(\b)" + keyword + R"(\b)"), "Keyword: " + keyword});
    }

    patterns.push_back({TokenType::Operator, std::regex(R"(->|=>|\.\.|:?)"), "Operator: ->, =>, ..?, etc."});
    patterns.push_back({TokenType::Operator, std::regex(R"(==|!=|<=|>=|\+=|-=|\*=|/=|%=|&&|\|\||<<|>>|=)"), "Operator: ==, !=, etc."});
    patterns.push_back({TokenType::Operator, std::regex(R"([+\-*\/%&|^!~<>] )"), "Operator: +, -, *, /, etc."});
    patterns.push_back({TokenType::Punctuation, std::regex(R"([{}();.,:\[\]])"), "Punctuation: {}, ();,."});

    patterns.push_back({TokenType::Identifier, std::regex(R"(r#_|[a-zA-Z_][a-zA-Z0-9_]*)"), "Identifier"});

    patterns.push_back({TokenType::Whitespace, std::regex(R"(\s+)"), "Whitespace"});
}

bool match(const std::string& code, int& pos, TokenType& type, std::string& value) {
    std::string cur_str = code.substr(pos);
    std::smatch m;
    value = "";
    for (const auto& pattern: patterns) {
        if (std::regex_search(cur_str, m, pattern.regex) && m.position() == 0) {
            if (value.length() < m.str().length()) {
                value = m.str();
                type = pattern.type;
            }
        }
    }
    pos += value.length();
    return value.length() > 0;
}

void skip_space(const std::string& code, int &pos) {
    while (pos < code.length() && isspace(code[pos])) pos++;
}

Token get_next_token(const std::string& code, int& pos) {
    skip_space(code, pos);

    if (pos >= code.length()) return Token(TokenType::Unknown, "", code.length());

    int old_pos = pos;
    TokenType match_type;
    std::string matched_value;
    if (match(code, pos, match_type, matched_value)) return Token(match_type, matched_value, old_pos);

    pos++;
    return Token(TokenType::Unknown, "Unknown char", pos - 1);
}

std::vector<Token> tokenizeRustCode(const std::string& code) {
    std::vector<Token> tokens;

    int pos = 0;
    while (pos < code.length()) {
        tokens.push_back(get_next_token(code, pos));
    }
    return tokens;
}

#endif //LEXER_H
