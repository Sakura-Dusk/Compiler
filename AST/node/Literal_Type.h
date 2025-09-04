//
// Created by Sakura on 25-8-27.
//

#ifndef LITERAL_TYPE_H
#define LITERAL_TYPE_H
#include<memory>
#include <variant>
#include <vector>
#include <map>

enum Atom_Literal_Type {
    CHAR,
    STRING,
    RAW_STRING,
    BYTE,
    BYTE_STRING,
    RAW_BYTE_STRING,
    C_STRING,
    RAW_C_STRING,
    INTEGER,
    FLOAT,
    BOOL,
};

class LiteralType {
    //Sequence types
    struct Tuple {
        std::vector<LiteralType> elements;
    };

    struct Array {
        size_t size;
        std::shared_ptr<LiteralType> elements;
    };

    struct Slice {
        std::shared_ptr<LiteralType> elements;
    };

    struct Union {
        std::vector<LiteralType> options;
    };

    struct Path {
        std::vector<std::string> paths;
    };

    std::variant<Atom_Literal_Type, Tuple, Array, Slice, Union, Path> type;

public:
    LiteralType() = default;
    LiteralType(std::variant<Atom_Literal_Type, Tuple, Array, Slice, Union, Path> type) : type(move(type)) {}

    LiteralType atom(Atom_Literal_Type x) {
        return LiteralType{x};
    }
    LiteralType tuple(std::vector<LiteralType> x) {
        return LiteralType{Tuple{std::move(x)}};
    }
    LiteralType array(const LiteralType& x, size_t s) {
        return LiteralType{Array{s, std::make_shared<LiteralType>(x)}};
    }
    LiteralType slice(const LiteralType& x) {
        return LiteralType{Slice{std::make_shared<LiteralType>(x)}};
    }
    LiteralType union_(std::vector<LiteralType> x) {
        return LiteralType{Union{x}};
    }
    LiteralType path(std::vector<std::string> x) {
        return LiteralType{Path{x}};
    }

    bool is_atom() const {
        return std::holds_alternative<Atom_Literal_Type>(type);
    }
    bool is_tuple() const {
        return std::holds_alternative<Tuple>(type);
    }
    bool is_array() const {
        return std::holds_alternative<Array>(type);
    }
    bool is_slice() const {
        return std::holds_alternative<Slice>(type);
    }
    bool is_union() const {
        return std::holds_alternative<Union>(type);
    }
    bool is_path() const {
        return std::holds_alternative<Path>(type);
    }

    Atom_Literal_Type get_atom() const {
        return std::get<Atom_Literal_Type>(type);
    }
    Tuple get_tuple() const {
        return std::get<Tuple>(type);
    }
    Array get_array() const {
        return std::get<Array>(type);
    }
    Slice get_slice() const {
        return std::get<Slice>(type);
    }
    Union get_union() const {
        return std::get<Union>(type);
    }
    Path get_path() const {
        return std::get<Path>(type);
    }

    bool operator==(const LiteralType& other) const {
        if (type.index() != other.type.index()) return false;
        if (is_atom()) return get_atom() == other.get_atom();
        if (is_tuple()) return get_tuple().elements == other.get_tuple().elements;
        if (is_array()) return get_array().size == other.get_array().size && get_array().elements == other.get_array().elements;
        if (is_slice()) return get_slice().elements == other.get_slice().elements;
        if (is_union()) return get_union().options == other.get_union().options;
        if (is_path()) return get_path().paths == other.get_path().paths;
        return false;//unknown
    }

    bool operator<(const LiteralType& other) const {
        if (type.index() != other.type.index()) return type.index() < other.type.index();
        if (is_atom()) return get_atom() < other.get_atom();
        if (is_tuple()) return get_tuple().elements < other.get_tuple().elements;
        if (is_array()) return get_array().size == other.get_array().size ? get_array().size < other.get_array().size : get_array().elements < other.get_array().elements;
        if (is_slice()) return get_slice().elements < other.get_slice().elements;
        if (is_union()) return get_union().options > other.get_union().options;
        if (is_path()) return get_path().paths < other.get_path().paths;
        return false;//unknown
    }
};

inline std::map<Atom_Literal_Type, std::string> type_to_string = {
    {CHAR, "CHAR"},
    {STRING, "STRING"},
    {RAW_STRING, "RAW_STRING"},
    {BYTE, "BYTE"},
    {BYTE_STRING, "BYTE_STRING"},
    {RAW_BYTE_STRING, "RAW_BYTE_STRING"},
    {C_STRING, "C_STRING"},
    {RAW_C_STRING, "RAW_C_STRING"},
    {INTEGER, "INTEGER"},
    {FLOAT, "FLOAT"},
    {BOOL, "BOOL"},
};

inline std::map<std::string, Atom_Literal_Type> string_to_type = {
    {"CHAR", CHAR},
    {"STRING", STRING},
    {"RAW_STRING", RAW_STRING},
    {"BYTE", BYTE},
    {"BYTE_STRING", BYTE_STRING},
    {"RAW_BYTE_STRING", RAW_BYTE_STRING},
    {"C_STRING", C_STRING},
    {"RAW_C_STRING", RAW_C_STRING},
    {"INTEGER", INTEGER},
    {"FLOAT", FLOAT},
    {"BOOL", BOOL},
};

std::string show(const LiteralType& x) {
    if (x.is_atom()) {
        const auto fnd = type_to_string.find(x.get_atom());
        if (fnd == type_to_string.end()) return "Unknown atom";
        return fnd->second;
    }
    if (x.is_tuple()) {
        const auto fnd = x.get_tuple();
        std::string res = "{";
        for (auto it : fnd) {
            res += show(it);
            res += ", ";
        }
        res += "}";
        return res;
     }
    if (x.is_array()) {
        const auto fnd = x.get_array();
        std::string res = "[";
        res += show(*fnd.elements);
        res += "; size = ";
        res += std::to_string(fnd.size());
        res += "]";
        return res;
    }
    if (x.is_slice()) {
        const auto fnd = x.get_slice();
        std::string res = "(";
        res += show(*fnd.elements);
        res += ")";
        return res;
    }
    if (x.is_union()) {
        const auto fnd = x.get_union();
        std::string res = "{(union)";
        res += show(*fnd.options);
        res += "}";
        return res;
    }
    if (x.is_path()) {
        const auto fnd = x.get_path();
        std::string res = "{(path)";
        bool fir = true;
        for (auto it : fnd) {
            if (!fir) res += "::";
            res += show(it);
            fir = false;
        }
        res += "}";
        return res;
    }
    return "Unknown type";
}

#endif //LITERAL_TYPE_H
