//
// Created by Sakura on 26-1-12.
//

#include "semantic.h"

#include <complex>

#include "../../cmake-build-release/_deps/googletest-src/googletest/include/gtest/internal/gtest-port.h"

void Show_vector_string(std::vector<std::string> s) {
    for (auto str: s) std::cout << str << std::endl;
}

std::string rename_in_dependency(AstNode* node) {
  	if (node->type == AstNodetype::Implementation) {
  		return "impl@" + rename_in_dependency(node->children[0]);
  	}
    if (node->type == AstNodetype::Struct || node->type == AstNodetype::Enumeration || node->type == AstNodetype::Type) {
        return "Type@" + node->value;
    }
	return node->value;
}

bool is_item(AstNode* node) {
    if (node->type == AstNodetype::Struct || node->type == AstNodetype::Enumeration || node->type == AstNodetype::Function || node->type == AstNodetype::ConstantItem)
        return true;
    return false;
}

//just like "i32" in code show as a type, it shows a type but its actual_type is a typetype, so we need to convert it to i32
NodeType& type_to_item(NodeType &a) {
  	if (a == Unit) return a;
    // std::cout << a.show() << std::endl;
    if (a.type != NodeTypeType::Type_of_Type) throw std::runtime_error("Semantic Error: expected type type!");
    return *a.inside_type;
}

//just go backwards
NodeType item_to_type(NodeType* a) {
 	if (*a == Unit) return *a;
	NodeType T(NodeTypeType::Type_of_Type, a, 0);
	return T;
}

NodeType item_to_type(const NodeType &a) {
 	if (a == Unit) return a;
	NodeType T(NodeTypeType::Type_of_Type, new NodeType(a), 0);
	return T;
}

NodeType give_ref(NodeType &a) {
    return NodeType(NodeTypeType::Amp, &a, 0);
}

NodeType give_mutref(NodeType &a) {
    return NodeType(NodeTypeType::Mut_Amp, &a, 0);
}

//first one is expected type, second one is expression type
void Expect_Type_match(const NodeType& type, const NodeType& expr_type, std::string RE_words, bool mut_removeable = true) {
  	if (type == expr_type || type == Never || type == Wildcard || expr_type == Wildcard) return;
    if (expr_type == Int && (type == I32 || type == U32 || type == Isize || type == Usize || type == UInt || type == IInt)) return;
    if (expr_type == IInt && (type == I32 || type == Isize)) return;
    if (expr_type == UInt && (type == U32 || type == Usize)) return;
    if (mut_removeable && expr_type.type == NodeTypeType::Amp && type.type == NodeTypeType::Mut_Amp)
      	return Expect_Type_match(*type.inside_type, *expr_type.inside_type, RE_words, false);
   	if (expr_type.type == NodeTypeType::Mut_Amp && type.type == NodeTypeType::Mut_Amp)
      	return Expect_Type_match(*type.inside_type, *expr_type.inside_type, RE_words, false);
    if (expr_type.type == NodeTypeType::Amp && type.type == NodeTypeType::Amp)
      	return Expect_Type_match(*type.inside_type, *expr_type.inside_type, RE_words, false);
    if (expr_type.type == NodeTypeType::Array && type.type == NodeTypeType::Array) {
        if (expr_type.item_length != type.item_length) throw std::runtime_error(RE_words);
        return Expect_Type_match(*type.inside_type, *expr_type.inside_type, RE_words, false);
    }
    throw std::runtime_error(RE_words);
}

bool is_number(const NodeType &A) {
    return A == Int || A == IInt || A == UInt || A == I32 || A == U32 || A == Isize || A == Usize;
}
bool is_numberB(const NodeType &A) {
    return is_number(A) || A == Bool;
}
void Check_as_Operator_Pair(const NodeType& A, const NodeType& B, std::string RE_words) {
    if (A == B) return ;
    if (A == Never) return ;
    if (A != Char && !is_numberB(A)) throw std::runtime_error(RE_words);
    if (B != I32 && B != U32 && B != Isize && B != Usize) throw std::runtime_error(RE_words);
}

void Number_Type_Pair(NodeType& A, NodeType& B, std::string RE_words) {
    if (!is_number(A) || !is_number(B)) throw std::runtime_error(RE_words + "! should be number but not!");
    if (A == Int) A = B;
    else if (A == IInt && B == Int) B = A;
    else if (A == IInt && (B == I32 || B == Isize)) A = B;
    else if (A == UInt && B == Int) B = A;
    else if (A == UInt && (B == U32 || B == Usize)) A = B;
    else if (A == I32 && (B == Int || B == IInt)) B = A;
    else if (A == U32 && (B == Int || B == UInt)) B = A;
    else if (A == Isize && (B == Int || B == IInt)) B = A;
    else if (A == Usize && (B == Int || B == UInt)) B = A;

    if (A != B) throw std::runtime_error(RE_words + "! number type mismatch!");
}

void Type_Pair(NodeType& A, NodeType& B, std::string RE_words) {
    if (A == Wildcard) {A = B; return ;}
    if (B == Wildcard) {B = A; return ;}
    if (A.type == NodeTypeType::Array || B.type == NodeTypeType::Array) {
        if (A.type != B.type || A.item_length != B.item_length)
            throw std::runtime_error(RE_words);
        Type_Pair(*A.inside_type, *B.inside_type, RE_words);
    }
    if (A != B) Number_Type_Pair(A, B, RE_words);
}

scope_item& find_scope_type(Scope* scope_value, const AstNode* scope_father, const std::string& name) {
    auto& res = scope_value->get_type(name);
    if (res.type != NodeTypeType::Unknown) return res;
    if (scope_father == nullptr) return res;
    return find_scope_type(scope_father->scope_value, scope_father->scope_father, name);
}

scope_item& find_scope_item(Scope* scope_value, const AstNode* scope_father, const std::string& name) {
    auto& res = scope_value->get_item(name);
    if (res.type != NodeTypeType::Unknown) return res;
    if (scope_father == nullptr) return res;
    return find_scope_item(scope_father->scope_value, scope_father->scope_father, name);
}

void Build_Dependency_Graph(AstNode* node, NodeType& current_return_type = UnKnown) {
  	if (current_return_type != UnKnown) {
    	for (auto son: node->children)
        	son->scope_value = node->scope_value, son->scope_father = node->scope_father;
  	}

    std::map <std::string, AstNode*> mp;
    std::vector <std::vector<int> > G;
    std::vector <int> du;

  	std::vector <std::string> name;
	for (auto son: node->children)
    	if (is_item(son)) name.push_back(rename_in_dependency(son));
    sort(name.begin(), name.end());

    G.resize(name.size(), std::vector<int>());
    du.resize(name.size(), 0);
    for (auto son: node->children)
      	if (is_item(son)) mp[rename_in_dependency(son)] = son;

    for (auto son: node->children) {
    	if (!is_item(son)) continue;
        std::vector <AstNode*> queue;
        for (auto child: son->children)
            queue.push_back(child);
        int now = lower_bound(name.begin(), name.end(), rename_in_dependency(son)) - name.begin();
        while (!queue.empty()) {
            auto now_node = queue.back();
            queue.pop_back();

            if (now_node->type == AstNodetype::Type || now_node->type == AstNodetype::Identifier) {
                std::string to_name = rename_in_dependency(now_node);
                int to = lower_bound(name.begin(), name.end(), to_name) - name.begin();
                if (to != name.size() && now == to && name[to] == to_name && mp[name[to]]->type != AstNodetype::Function) {
                    G[to].push_back(now); du[now]++;
                }
            }

            for (auto to: now_node->children)
                queue.push_back(to);
        }
    }

    std::vector <int> ord;
    std::vector <int> queue;
    for (int i = 0; i < name.size(); i++)
      	if (!du[i]) queue.push_back(i);
    while (!queue.empty()) {
        int now = queue.back();
        queue.pop_back();
        ord.push_back(now);
        for (auto to: G[now]) {
            du[to]--;
            if (!du[to]) queue.push_back(to);
        }
    }
	if (ord.size() != name.size()) throw std::runtime_error("Semantic Error: cyclic dependency detected!");

	auto tmp = node->children;
    node->children.clear();
    for (auto id: ord)
        node->children.push_back(mp[name[id]]);
   	for (auto son: tmp)
        if (son->type == AstNodetype::Implementation)
            node->children.push_back(son);
    for (auto son: tmp)
        if (!is_item(son) && son->type != AstNodetype::Implementation)
            node->children.push_back(son);

    //then build sons' dependency graph
    for (auto son: node->children) {
      	if (son->type == AstNodetype::Function) {
            son->children[0]->scope_value = son->scope_value;
            son->children[0]->scope_father = son->scope_father;
            son->scope_value->add_item("self", scope_item(Unit, std::any(), false, false, ++Item_id_tot));
            semantic_visit_node(son->children[0], nullptr, nullptr, nullptr);
            son->scope_value->item_table.erase("self");

            //FunctionReturnType
            son->children[1]->scope_value = son->scope_value;
            son->children[1]->scope_father = son->scope_father;
            semantic_visit_node(son->children[1], nullptr, nullptr, nullptr);

            NodeType return_type = son->children[1]->actual_type;
            NodeType T(NodeTypeType::Function, new NodeType(type_to_item(return_type)), -1);
            son->scope_value = new Scope();
            son->scope_father = node;
            if (node->scope_value->get_item(son->value).is_uncoverable == true) {
            	throw std::runtime_error("Semantic Error: redefinition of function " + son->value + "!");
            }
            for (auto grand_son: son->children[0]->children) {
                bool has_ref = false, has_mut = false;
                for (auto to: grand_son->children) {
                    if (to->type == AstNodetype::Quantifier && to->value == "&") has_ref = true;
                    if (to->type == AstNodetype::Quantifier && to->value == "mut") has_mut = true;
                }
                if (grand_son->type != AstNodetype::Self) {
                    NodeType param_type = grand_son->children.back()->actual_type;
                    if (has_ref) throw std::runtime_error("Semantic Error: function parameter cannot be reference type!");
                    if (has_mut) son->scope_value->add_item(grand_son->value, scope_item(param_type, std::any(), true, false, ++Item_id_tot));
                    else son->scope_value->add_item(grand_son->value, scope_item(param_type, std::any(), false, false, ++Item_id_tot));
                    grand_son->actual_type = param_type;
                    grand_son->variableID = Item_id_tot;
                    T.items_type.push_back(new NodeType(param_type));
                }
                else {
                    if (grand_son != son->children[0]->children[0]) throw std::runtime_error("Semantic Error: self parameter must be the first parameter!");
                    if (current_return_type == UnKnown) throw std::runtime_error("Semantic Error: self parameter can only appear in method!");

                    NodeType &self_type = current_return_type;
                    if (has_mut) T.is_mutable = true;
                    if (has_ref && has_mut)
                    son->scope_value->add_item("self", scope_item(grand_son->actual_type = give_mutref(current_return_type), std::any(), false, false, ++Item_id_tot));
                    if (has_ref && !has_mut)
                    son->scope_value->add_item("self", scope_item(grand_son->actual_type = give_ref(current_return_type), std::any(), false, false, ++Item_id_tot));
                    if (!has_ref && has_mut)
                    son->scope_value->add_item("self", scope_item(grand_son->actual_type = current_return_type, std::any(), true, false, ++Item_id_tot));
                    if (!has_ref && !has_mut) {
                        if (grand_son->children.size()) {
                            auto need_return_type = grand_son->children.back()->actual_type;
                            if (need_return_type != current_return_type)
                            throw std::runtime_error("Semantic Error: self parameter type mismatch!");
                        }

                        son->scope_value->add_item("self", scope_item(grand_son->actual_type = current_return_type, std::any(), false, false, ++Item_id_tot));
                    }

                    grand_son->variableID = Item_id_tot;
                    T.type = NodeTypeType::Method;
                    T.self_type = &son->scope_value->get_item("self").type;
                }
            }

            auto si = scope_item(T, std::any(), false, true, 1);
            if (!(node->type == AstNodetype::Program && son->value == "main")) {
             	si.ID = ++Item_id_tot;
            }
            node->scope_value->add_item(son->value, si);

            son->variableID = si.ID;
    	}
        if (son->type == AstNodetype::Struct) {
            NodeType* T = new NodeType(NodeTypeType::Struct);
            T->FM_id = ++Item_id_tot;
            T->SE_name = son->value;
            T->field = new Scope();
			T->items_index = new std::map<std::string, unsigned int>();
            son->scope_value = T->field;
            son->scope_father = node;
            if (son->children.size()) {
                son->children[0]->scope_value = son->scope_value;
                son->children[0]->scope_father = son->scope_father;
                semantic_visit_node(son->children[0], nullptr, nullptr, nullptr);
                for (auto grand_son: son->children[0]->children) {
                    auto &each_type = type_to_item(grand_son->children[0]->actual_type);
                    son->scope_value->add_item("@" + grand_son->value, scope_item(each_type, std::any(), true, true, ++Item_id_tot));
                    T->items_index->insert({grand_son->value, T->items_index->size()});
                    T->items_type.push_back(&each_type);
                }
            }
            if (node->scope_value->get_type(son->value).is_uncoverable == true) {
            	throw std::runtime_error("Semantic Error: redefinition of struct " + son->value + "!");
            }
            node->scope_value->add_type(son->value, scope_item(item_to_type(T), std::any(), false, true, T->FM_id));
            son->actual_type = item_to_type(T);
        }
        if (son->type == AstNodetype::Enumeration) {
            NodeType* T = new NodeType(NodeTypeType::Enum);
            T->FM_id = ++Item_id_tot;
            T->SE_name = son->value;
            T->items_index = new std::map<std::string, unsigned int>();
            for (int i = 0; i < son->children.size(); i++) {
                auto elements = son->children[i];
                if (T->items_index->contains(elements->value))
                    throw std::runtime_error("Semantic Error: redefinition of enumeration item " + elements->value + "!");
                T->items_index->insert({elements->value, i});
            }
            if (node->scope_value->get_type(son->value).is_uncoverable == true) {
            	throw std::runtime_error("Semantic Error: redefinition of enum " + son->value + "!");
            }
            node->scope_value->add_type(son->value, scope_item(item_to_type(T), std::any(), false, true, T->FM_id));
        }
        if (son->type == AstNodetype::ConstantItem) {
			son->children[0]->scope_value = son->scope_value;
            son->children[0]->scope_father = son->scope_father;
            semantic_visit_node(son->children[0], nullptr, nullptr, nullptr);

            son->children[1]->scope_value = son->scope_value;
            son->children[1]->scope_father = son->scope_father;
            semantic_visit_node(son->children[1], nullptr, nullptr, nullptr);

            NodeType const_type = type_to_item(son->children[0]->actual_type), expr_type = son->children[1]->actual_type;
            Expect_Type_match(const_type, expr_type, "constant item " + son->value + " type mismatch!");

            if (!son->children[1]->const_value.has_value())
            	throw std::runtime_error("Semantic Error: constant item " + son->value + " value is constant expression but no initialize value!");

            if (node->scope_value->get_item(son->value).is_uncoverable == true) {
            	throw std::runtime_error("Semantic Error: redefinition of constant item " + son->value + "!");
            }
            node->scope_value->add_item(son->value, scope_item(const_type, son->children[1]->const_value, false, true, ++Item_id_tot));
        }
        if (son->type == AstNodetype::Implementation) {
            NodeType& T = type_to_item(son->scope_value->get_type(son->value).type);
            if (T.type != NodeTypeType::Struct) throw std::runtime_error("Semantic Error: implementation target must be struct type!");
            son->scope_value = T.field;
            son->scope_father = node;
            T.field->add_type("Self", scope_item(item_to_type(T), std::any(), false, false, ++Item_id_tot));
            son->children[0]->scope_value = son->scope_value;
            son->children[0]->scope_father = son->scope_father;
            semantic_visit_node(son->children[0], nullptr, nullptr, nullptr);
        }
    }
}

void semantic_visit_node(AstNode* node, AstNode* father = nullptr, AstNode* loop_father = nullptr, AstNode* function_father = nullptr) {
    node->father = father;
    if (node->actual_type.type != NodeTypeType::Unknown) return ;//already done
    if (node->type == AstNodetype::Statements) {
    	node->scope_value = new Scope();
        node->scope_father = father;
    }
    for (auto child : node->children)//other normal subtree same as root as a unit scope
        child->scope_value = node->scope_value, child->scope_father = node->scope_father;

    if (node->type == AstNodetype::Program || node->type == AstNodetype::Statements) {
        Build_Dependency_Graph(node);
    }

    if (node->type == AstNodetype::Function) {
		function_father = node; loop_father = nullptr;
    }
    if (node->type == AstNodetype::Loop || node->type == AstNodetype::While) {
        loop_father = node;
    }

    //first Semantic check children
    if (node->type == AstNodetype::Binary_Operator && (node->value == "." || node->value == "::")) semantic_visit_node(node->children[0], node, loop_father, function_father);
    else {
        for (int i = 0; i < node->children.size(); i++) {
          	auto child = node->children[i];
			node->now_go_son_id = i;
            semantic_visit_node(child, node, loop_father, function_father);
            node->exist_break |= child->exist_break;
            node->exist_return |= child->exist_return;
            node->must_break |= child->must_break;
        }
    }

    // std::cout << "start semantic check node : " << std::endl;
    // auto opt_str = node->show_node();
    // for (auto str: opt_str) std::cout << str << std::endl;
    // std::cout << "----------\n";

    //then update state about exist_return, exist_break, must_break
    if (node->type == AstNodetype::Function) {
        node->exist_return = false;
    }
    if (node->type == AstNodetype::While) {//You might don't go inside while so cannot must exist return
        node->exist_return = false;
    }
    if (node->type == AstNodetype::Loop || node->type == AstNodetype::While) {
        node->must_break = false; node->exist_break = false;
    }
    if (node->type == AstNodetype::If) {
        if (node->children.size() == 3) {
            node->exist_break = node->children[1]->exist_break && node->children[2]->exist_break;
            node->exist_return = node->children[1]->exist_return && node->children[2]->exist_return;
        }
        else {
            node->exist_break = false;
            node->exist_return = false;
        }
    }
    if (node->type == AstNodetype::Break) node->exist_break = true, node->must_break = true;
    if (node->type == AstNodetype::Return) node->exist_return = true;
    if (node->type == AstNodetype::Continue) node->exist_break = true;

	if (node->type == AstNodetype::Function) {
        auto return_type = type_to_item(node->children[1]->actual_type);
	    auto func_type = node->children[2]->actual_type;
	    Expect_Type_match(return_type, func_type, "function " + node->value + " return type mismatch!");
    }
    else if (node->type == AstNodetype::Identifier) {
        scope_item s;
        if (father->type == AstNodetype::Binary_Operator && father->value == "::" && father->now_go_son_id == 0)
            s = find_scope_type(node->scope_value, node->scope_father, node->value);
        else if (father->type == AstNodetype::Binary_Operator && father->value == "as" && father->now_go_son_id == 1)
            s = find_scope_type(node->scope_value, node->scope_father, node->value);
        else if (father->type == AstNodetype::StructField && father->now_go_son_id == 0)
            s = find_scope_type(node->scope_value, node->scope_father, node->value);
        else {
            s = find_scope_item(node->scope_value, node->scope_father, node->value);
            if (s.type == UnKnown) s = find_scope_type(node->scope_value, node->scope_father, node->value);
        }

        if (s.type == UnKnown) {
            throw std::runtime_error("Semantic Error: identifier " + node->value + " not found in field!");
        }

        node->is_mut = s.is_mutable;
        node->is_variable = true;
        node->actual_type = s.type;
        node->const_value = s.const_value;
        node->variableID = s.ID;

        if (s.type.is_exit && father->type == AstNodetype::FunctionCall && father->now_go_son_id == 0) {
            auto now_node = father->father;
            if (now_node == nullptr) throw std::runtime_error("Semantic Error: exit function must be called in main function body!");
            if (now_node->type != AstNodetype::Expression && now_node->type != AstNodetype::Return) throw std::runtime_error("Semantic Error: exit function must be called in main function body!");

            now_node = now_node->father;
            if (now_node == nullptr) throw std::runtime_error("Semantic Error: exit function must be called in main function body!");
            if (now_node->type != AstNodetype::Statements || now_node->now_go_son_id != now_node->children.size() - 1) throw std::runtime_error("Semantic Error: exit function must be called at the end of function body!");

            now_node = now_node->father;
            if (now_node == nullptr) throw std::runtime_error("Semantic Error: exit function must be called in main function body!");
            if (now_node->type != AstNodetype::Function || now_node->value != "main") throw std::runtime_error("Semantic Error: exit function must be called in main function body!");

            now_node = now_node->father;
            if (now_node == nullptr) throw std::runtime_error("Semantic Error: exit function must be called in main function body!");
            if (now_node->type != AstNodetype::Program) throw std::runtime_error("Semantic Error: exit function must be called in main function body!");
        }
    }
    else if (node->type == AstNodetype::Self) {
        auto &s = find_scope_item(node->scope_value, node->scope_father, "self");
        if (s.type == UnKnown) throw std::runtime_error("Semantic Error: self identifier not found in field!");
        node->actual_type = s.type;
        node->is_mut = s.is_mutable;
        node->is_variable = true;
        node->variableID = s.ID;
    }
    else if (node->type == AstNodetype::Type) {
        if (node->value == "i32") node->actual_type = I32_Type;
        else if (node->value == "u32") node->actual_type = U32_Type;
        else if (node->value == "isize") node->actual_type = Isize_Type;
        else if (node->value == "usize") node->actual_type = Usize_Type;
        else if (node->value == "str") node->actual_type = Str_Type;
        else if (node->value == "string") node->actual_type = String_Type;
        else if (node->value == "char") node->actual_type = Char_Type;
        else if (node->value == "bool") node->actual_type = Bool_Type;
        else if (node->value == "()") node->actual_type = Unit;
        else if (node->value == "_") node->actual_type = Wildcard_Type;
        else {
            if (node->children.empty()) {//sth like StructName
                auto s = find_scope_type(node->scope_value, node->scope_father, node->value).type;
                if (s.type != NodeTypeType::Type_of_Type) throw std::runtime_error("Semantic Error: type " + node->value + " not found in field!");
                node->actual_type = s;
            }
            else if (node->children.size() == 2) {// Array
                //first children is type, second is length expression
                auto const_length = node->children[1]->const_value;
                auto const_type = node->children[1]->actual_type;
                if (!const_length.has_value() || (const_type != UInt && const_type != Int && const_type != Usize)) throw std::runtime_error("Semantic Error: array length must be constant unsigned integer expression!");
                node->actual_type.type = NodeTypeType::Type_of_Type;
                node->actual_type.inside_type = new NodeType(NodeTypeType::Array, node->children[0]->actual_type.inside_type, static_cast<unsigned int>(std::any_cast<long long>(const_length)));
            }
            else throw std::runtime_error("Semantic Error: unknown type expression!");
        }
    }
    else if (node->type == AstNodetype::Statements) {
        if (node->children.empty()) {
            node->actual_type = Unit;
        }
        else {
            if (node->children.back()->type == AstNodetype::Return_Cur) {
                node->actual_type = node->children.back()->actual_type;
            }
            else {
                node->actual_type = Unit;
                for (auto son: node->children)
                    if (son->actual_type == Never || son->exist_break || son->exist_return) node->actual_type == Never;
            }
            for (auto son: node->children) if (son != node->children.back())
                if (son->type == AstNodetype::Return_Cur && son->actual_type != Unit && son->actual_type != Never)
                    throw std::runtime_error("Semantic Error: unknown return type expression!");
        }
    }
    else if (node->type == AstNodetype::Return_Cur) {
        node->actual_type = node->children[0]->actual_type;
        node->const_value = node->children[0]->const_value;
    }
    else if (node->type == AstNodetype::LetStatement) {
        // Show_vector_string(node->children[1]->show_node());
        auto var_type = type_to_item(node->children[1]->actual_type);
        auto expr_type = node->children[2]->actual_type;
        Expect_Type_match(var_type, expr_type, "let statement " + node->children[0]->value + " type mismatch!");
        if (var_type == Wildcard) {
            var_type = expr_type;
            if (expr_type == Int || expr_type == IInt) var_type = I32;
            if (expr_type == Never) var_type = Unit;
        }
        scope_item value(var_type, std::any(), false, false, ++Item_id_tot);
        if (node->children[0]->value == "mut") value.is_mutable = true;
        node->actual_type = var_type;
        node->variableID = value.ID;
        auto item = find_scope_item(node->scope_value, node->scope_father, node->value);
        if (item.is_uncoverable) throw std::runtime_error("Semantic Error: redefinition of a uncoverable variable " + node->value + "!");
        if (node->value != "_") node->scope_value->add_item(node->value, value);
    }
    else if (node->type == AstNodetype::Expression) {
        if (node->children.empty()) {
            node->actual_type = Unit;
            node->const_value = Unit_value();
        }
        else {
            node->actual_type = node->children[0]->actual_type;
            node->const_value = node->children[0]->const_value;
            node->is_mut = node->children[0]->is_mut;
            node->is_variable = node->children[0]->is_variable;
        }
    }
    else if (node->type == AstNodetype::ArrayElements) {
        if (node->children.size() == 1) {//[a,b,c]
            auto& items = node->children[0]->children;
            if (items.empty()) throw std::runtime_error("Semantic Error: cannot infer type of empty array!");
            NodeType& item_type = items[0]->actual_type;
            std::vector<std::any> const_value;
            bool is_const = true;
            for (auto item: items) {
                if (item_type == Never) item_type = item->actual_type;
                else if (item->actual_type != Never) Type_Pair(item->actual_type, item_type, "array elements type mismatch!");
                if (!item->const_value.has_value()) is_const = false;
                if (is_const) const_value.push_back(item->const_value);
            }
            node->actual_type = (NodeType){NodeTypeType::Array, &item_type, static_cast<unsigned int>(items.size())};
            if (is_const) node->const_value = const_value;
        }
        else {
            //[a;b]
            auto length_type = node->children[1]->actual_type;
            auto const_length = node->children[1]->const_value;
            if (!const_length.has_value() || (length_type != Int && length_type != UInt && length_type != Usize))
                throw std::runtime_error("Semantic Error: array length must be constant unsigned integer expression!");
            unsigned int length = static_cast<unsigned int>(std::any_cast<long long>(const_length));
            node->actual_type = (NodeType){NodeTypeType::Array, &node->children[0]->actual_type, length};
            auto const_value_single = node->children[0]->const_value;
            if (const_value_single.has_value()) {
                std::vector<std::any> const_value;
                for (unsigned int i = 0; i < length; i++)
                    const_value.push_back(const_value_single);
                node->const_value = const_value;
            }
        }
    }
    else if (node->type == AstNodetype::Loop) {
        if (node->actual_type == UnKnown) node->actual_type = Unit;
        if (!node->children[0]->must_break) node->actual_type = Never;
        Expect_Type_match(Unit, node->children[0]->actual_type, "loop body type mismatch!");
    }
    else if (node->type == AstNodetype::While) {
        if (node->children[0]->actual_type != Bool) throw std::runtime_error("Semantic Error: while condition must be boolean expression!");
        node->actual_type = Unit;
    }
    else if (node->type == AstNodetype::If) {
        if (node->children[0]->actual_type != Bool) throw std::runtime_error("Semantic Error: if condition must be boolean expression!");
        if (node->children.size() <= 2) {
            Expect_Type_match(Unit, node->children[1]->actual_type, "if body type should be Unit type!");
            node->actual_type = Unit;
        }
        else {
            auto true_type = node->children[1]->actual_type;
            auto false_type = node->children[2]->actual_type;
            if (true_type == Never) true_type = false_type;
            else if (false_type == Never) false_type = true_type;
            Type_Pair(true_type, false_type, "if-else body type mismatch!");
            node->actual_type = true_type;
        }
    }
    else if (node->type == AstNodetype::Else) {
        if (father->type != AstNodetype::If) throw std::runtime_error("Semantic Error: else without if!");
        node->actual_type = node->children[0]->actual_type;
        node->const_value = node->children[0]->const_value;
    }
    else if (node->type == AstNodetype::Unary_Operator) {
        auto &operand_type = node->children[0]->actual_type;
        auto const_value = node->children[0]->const_value;
        if (node->value == "-") {
            auto should_type = IInt;
            Expect_Type_match(should_type, operand_type, "unary - operator type mismatch!");
            node->actual_type = operand_type;
            if (const_value.has_value()) node->const_value = -std::any_cast<long long>(const_value);
        }
        else if (node->value == "!") {
            if (!is_numberB(operand_type)) throw std::runtime_error("Semantic Error: unary ! operator type mismatch!");
            node->actual_type = operand_type;
            if (const_value.has_value()) {
                if (operand_type == Bool) node->const_value = !std::any_cast<bool>(const_value);
                else node->const_value = ~std::any_cast<long long>(const_value);
            }
        }
        else if (node->value == "&") {
            if (operand_type.type == NodeTypeType::Type_of_Type) node->actual_type = item_to_type(give_ref(type_to_item(operand_type)));
            else node->actual_type = give_ref(operand_type);
        }
        else if (node->value == "&mut") {
            if (operand_type.type == NodeTypeType::Type_of_Type) node->actual_type = item_to_type(give_mutref(type_to_item(operand_type)));
            else node->actual_type = give_mutref(operand_type);
            if (node->children[0]->is_mut == false && node->children[0]->is_variable == true)
                throw std::runtime_error("Semantic Error: cannot take mutable reference of immutable variable!");
        }
        else if (node->value == "*") {
            if (operand_type.type != NodeTypeType::Amp && operand_type.type != NodeTypeType::Mut_Amp)
                throw std::runtime_error("Semantic Error: dereference operator type mismatch!");
            if (operand_type.type == NodeTypeType::Mut_Amp) node->is_mut = true;
                else node->is_mut = false;
            node->is_variable = true;
            node->actual_type = *operand_type.inside_type;
        }
    }
    else if (node->type == AstNodetype::Binary_Operator) {
        auto type1 = node->children[0]->actual_type;
        auto type2 = node->children[1]->actual_type;
        auto value1 = node->children[0]->const_value;
        auto value2 = node->children[1]->const_value;

        if (node->value == "+" || node->value == "-" || node->value == "*" || node->value == "/" || node->value == "%") {
            if (type1.type == NodeTypeType::Amp) type1 = *type1.inside_type;
            if (type2.type == NodeTypeType::Amp) type2 = *type2.inside_type;
            if (!is_numberB(type1) || !is_numberB(type2))
                throw std::runtime_error("Semantic Error: arithmetic operator type mismatch!");
            Number_Type_Pair(type1, type2, "arithmetic operator type mismatch!");
            node->actual_type = type1;
            if (value1.has_value() && value2.has_value()) {
                auto const_value1 = std::any_cast<long long>(value1);
                auto const_value2 = std::any_cast<long long>(value2);
                if (node->value == "+") node->const_value = const_value1 + const_value2;
                else if (node->value == "-") node->const_value = const_value1 - const_value2;
                else if (node->value == "*") node->const_value = const_value1 * const_value2;
                else if (node->value == "/") {
                    if (const_value2 == 0) throw std::runtime_error("Semantic Error: division by zero!");
                    node->const_value = const_value1 / const_value2;
                }
                else if (node->value == "%") {
                    if (const_value2 == 0) throw std::runtime_error("Semantic Error: modulo by zero!");
                    node->const_value = const_value1 % const_value2;
                }
            }
            node->variableID = ++Item_id_tot;
        }
        else if (node->value == "<<" || node->value == ">>") {
            if (type1.type == NodeTypeType::Amp) type1 = *type1.inside_type;
            if (type2.type == NodeTypeType::Amp) type2 = *type2.inside_type;
            if (!is_number(type1) || !is_number(type2))
                throw std::runtime_error("Semantic Error: bitwise shift operator type mismatch!");
            node->actual_type = type1;
            if (value1.has_value() && value2.has_value()) {
                auto const_value1 = std::any_cast<long long>(value1);
                auto const_value2 = std::any_cast<long long>(value2);
                if (const_value2 >= 32) throw std::runtime_error("Semantic Error: bitwise shift count too large(>=32)!");
                if (const_value2 < 0) throw std::runtime_error("Semantic Error: bitwise shift count negative!!");
                if (node->value == "<<") node->const_value = const_value1 << const_value2;
                else if (node->value == ">>") node->const_value = const_value1 >> const_value2;
            }
            node->variableID = ++Item_id_tot;
        }
        else if (node->value == "&" || node->value == "|" || node->value == "^") {
            if (type1.type == NodeTypeType::Amp) type1 = *type1.inside_type;
            if (type2.type == NodeTypeType::Amp) type2 = *type2.inside_type;
            if (type1 == Bool && type2 == Bool) {
                node->actual_type = Bool;
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<bool>(value1);
                    auto const_value2 = std::any_cast<bool>(value2);
                    if (node->value == "&") node->const_value = const_value1 & const_value2;
                    else if (node->value == "|") node->const_value = const_value1 | const_value2;
                    else if (node->value == "^") node->const_value = const_value1 ^ const_value2;
                }
            }
            else {
                Number_Type_Pair(type1, type2, "bitwise operator type mismatch!");
                node->actual_type = type1;
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<long long>(value1);
                    auto const_value2 = std::any_cast<long long>(value2);
                    if (node->value == "&") node->const_value = const_value1 & const_value2;
                    else if (node->value == "|") node->const_value = const_value1 | const_value2;
                    else if (node->value == "^") node->const_value = const_value1 ^ const_value2;
                }
            }
            node->variableID = ++Item_id_tot;
        }
        else if (node->value == "==" || node->value == "!=") {
            while ((type1.type == NodeTypeType::Amp || type1.type == NodeTypeType::Mut_Amp) && (type2.type == NodeTypeType::Amp || type2.type == NodeTypeType::Mut_Amp)) {
                type1 = *type1.inside_type;
                type2 = *type2.inside_type;
            }
            node->actual_type = Bool;

            if (type1 == Char && type2 == type1) {
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<char>(value1);
                    auto const_value2 = std::any_cast<char>(value2);
                    if (node->value == "==") node->const_value = (const_value1 == const_value2);
                    else if (node->value == "!=") node->const_value = (const_value1 != const_value2);
                }
            }
            else if (type1 == Bool && type2 == type1) {
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<bool>(value1);
                    auto const_value2 = std::any_cast<bool>(value2);
                    if (node->value == "==") node->const_value = (const_value1 == const_value2);
                    else if (node->value == "!=") node->const_value = (const_value1 != const_value2);
                }
            }
            else if ((type1 == Never || type2 == Unit) && (type2 == Never || type1 == Unit)) {
                if (value1.has_value() && value2.has_value()) {
                    if (node->value == "==") node->const_value = true;
                    else if (node->value == "!=") node->const_value = false;
                }
            }
            else if (type1.type == NodeTypeType::Enum && type2.type == NodeTypeType::Enum) {
                if (type1.FM_id != type2.FM_id) throw std::runtime_error("Semantic Error: enum type mismatch in equality operator!");
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<long long>(value1);
                    auto const_value2 = std::any_cast<long long>(value2);
                    if (node->value == "==") node->const_value = (const_value1 == const_value2);
                    else if (node->value == "!=") node->const_value = (const_value1 != const_value2);
                }
            }
            else {
                Number_Type_Pair(type1, type2, "bitwise operator type mismatch!");
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<long long>(value1);
                    auto const_value2 = std::any_cast<long long>(value2);
                    if (node->value == "==") node->const_value = (const_value1 == const_value2);
                    else if (node->value == "!=") node->const_value = (const_value1 != const_value2);
                }
            }

            node->variableID = ++Item_id_tot;
        }
        else if (node->value == "<" || node->value == "<=" || node->value == ">" || node->value == ">=") {
            while ((type1.type == NodeTypeType::Amp || type1.type == NodeTypeType::Mut_Amp) && (type2.type == NodeTypeType::Amp || type2.type == NodeTypeType::Mut_Amp)) {
                type1 = *type1.inside_type;
                type2 = *type2.inside_type;
            }
            node->actual_type = Bool;

            if (type1 == Char && type2 == type1) {
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<char>(value1);
                    auto const_value2 = std::any_cast<char>(value2);
                    if (node->value == "<") node->const_value = (const_value1 < const_value2);
                    else if (node->value == "<=") node->const_value = (const_value1 <= const_value2);
                    else if (node->value == ">") node->const_value = (const_value1 > const_value2);
                    else if (node->value == ">=") node->const_value = (const_value1 >= const_value2);
                }
            }
            else if (type1 == Bool && type2 == type1) {
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<bool>(value1);
                    auto const_value2 = std::any_cast<bool>(value2);
                    if (node->value == "<") node->const_value = (const_value1 < const_value2);
                    else if (node->value == "<=") node->const_value = (const_value1 <= const_value2);
                    else if (node->value == ">") node->const_value = (const_value1 > const_value2);
                    else if (node->value == ">=") node->const_value = (const_value1 >= const_value2);
                }
            }
            else {
                Number_Type_Pair(type1, type2, "relational operator type mismatch!");
                if (value1.has_value() && value2.has_value()) {
                    auto const_value1 = std::any_cast<long long>(value1);
                    auto const_value2 = std::any_cast<long long>(value2);
                    if (node->value == "<") node->const_value = (const_value1 < const_value2);
                    else if (node->value == "<=") node->const_value = (const_value1 <= const_value2);
                    else if (node->value == ">") node->const_value = (const_value1 > const_value2);
                    else if (node->value == ">=") node->const_value = (const_value1 >= const_value2);
                }
            }

            node->variableID = ++Item_id_tot;
        }
        else if (node->value == "&&" || node->value == "||") {
            if (type1.type == NodeTypeType::Amp) type1 = *type1.inside_type;
            if (type2.type == NodeTypeType::Amp) type2 = *type2.inside_type;
            if (type1 != type2) throw std::runtime_error("Semantic Error: logical operator type mismatch!");
            if (type1 != Bool) throw std::runtime_error("Semantic Error: logical operator type mismatch!");
            node->actual_type = Bool;
            if (value1.has_value() && value2.has_value()) {
                auto const_value1 = std::any_cast<bool>(value1);
                auto const_value2 = std::any_cast<bool>(value2);
                if (node->value == "&&") node->const_value = const_value1 && const_value2;
                else if (node->value == "||") node->const_value = const_value1 || const_value2;
            }

            node->variableID = ++Item_id_tot;
        }
        else if (node->value == "=") {
            if (node->children[0]->is_mut == false)
                throw std::runtime_error("Semantic Error: cannot assign to immutable variable!");
            if (type1.type == NodeTypeType::Amp || type2.type == NodeTypeType::Mut_Amp)
                type2.type = NodeTypeType::Amp;
            Expect_Type_match(type1, type2, "assignment operator type mismatch!");
            node->actual_type = Unit;
        }
        else if (node->value == "+=" || node->value == "-=" || node->value == "*=" || node->value == "/=" || node->value == "%=" || node->value == "<<=" || node->value == ">>=") {
            if (type1.type == NodeTypeType::Amp) type1 = *type1.inside_type;
            if (node->children[0]->is_mut == false)
                throw std::runtime_error("Semantic Error: cannot assign to immutable variable!");
            Number_Type_Pair(type1, type2, "relational operator type mismatch!");
            node->actual_type = Unit;
        }
        else if (node->value == "&=" || node->value == "|=" || node->value == "^=") {
            if (type1.type == NodeTypeType::Amp) type1 = *type1.inside_type;
            if (node->children[0]->is_mut == false)
                throw std::runtime_error("Semantic Error: cannot assign to immutable variable!");
            Type_Pair(type1, type2, "relational operator type mismatch!");
            node->actual_type = Unit;
        }
        else if (node->value == "::") {
            auto Type = node->children[0]->actual_type;
            while (Type.type == NodeTypeType::Amp || Type.type == NodeTypeType::Mut_Amp)
                Type = *Type.inside_type;
            if (Type.type != NodeTypeType::Type_of_Type || (Type.inside_type->type != NodeTypeType::Struct && Type.inside_type->type != NodeTypeType::Enum))
                throw std::runtime_error("Semantic Error: only struct or enum type has associated items!");
            Type = *Type.inside_type;
            if (Type.type == NodeTypeType::Struct) {//Struct
                auto value = Type.field->get_item(node->children[1]->value);
                if (value.type == UnKnown) throw std::runtime_error("Semantic Error: struct has no associated item " + node->children[1]->value + "!");
                if (value.type.type == NodeTypeType::Method) {
                    value.type.type = NodeTypeType::Function;
                    value.type.self_type = nullptr;
                    value.type.items_type.push_back(value.type.self_type);
                }
                node->actual_type = value.type;
                node->is_variable = node->children[1]->is_variable;
                node->const_value = value.const_value;
                node->variableID = value.ID;
            }
            else {//Enum
                if (!Type.items_index->contains(node->children[1]->value)) throw std::runtime_error("Semantic Error: enum has no variant " + node->children[1]->value + "!");
                node->actual_type = Type;
                node->const_value = (long long)(*Type.items_index)[node->children[1]->value];
            }
        }
        else if (node->value == ".") {
            auto Type = node->children[0]->actual_type;
            node->is_mut = node->children[0]->is_mut;
            node->is_variable = node->children[0]->is_variable;
            while (Type.type == NodeTypeType::Amp || Type.type == NodeTypeType::Mut_Amp) {
                if (Type.type == NodeTypeType::Mut_Amp) node->is_mut = true;
                else node->is_mut = false;
                Type = *Type.inside_type;
            }
            if (Type.type == NodeTypeType::Array && node->children[1]->type == AstNodetype::Identifier && node->children[1]->value == "len") {//function .len
                node->actual_type = (NodeType){NodeTypeType::Function, &Usize, 0};
                node->variableID = 1;
            }
            else if ((Type == U32 || Type == Usize || Type == UInt || Type == Int) && node->children[1]->type == AstNodetype::Identifier && node->children[1]->value == "to_string") {
                if (node->children[0]->const_value.has_value() && node->father->type == AstNodetype::FunctionCall) {
                    node->father->const_value = std::to_string(std::any_cast<long long>(node->children[0]->const_value));
                }
                node->actual_type = (NodeType){NodeTypeType::Function, &String, 0};
            }
            else {
                if (Type.type != NodeTypeType::Struct) throw std::runtime_error("Semantic Error: only struct type has fields!");
                if (node->children[1]->type != AstNodetype::Identifier) throw std::runtime_error("Semantic Error: invalid struct field access!");

                auto value = Type.field->get_item("@" + node->children[1]->value);
                if (value.type == UnKnown || father->type == AstNodetype::FunctionCall) {
                    value = Type.field->get_item(node->children[1]->value);
                    if (value.type == UnKnown || value.type.type != NodeTypeType::Method) throw std::runtime_error("Semantic Error: struct has no field " + node->children[1]->value + "!");
                }
                if (value.type.type == NodeTypeType::Method) {
                    if (value.type.is_mutable && !node->is_mut && node->is_variable) throw std::runtime_error("Semantic Error: cannot call mutable method on immutable variable!");
                    value.type.type = NodeTypeType::Function;
                    value.type.self_type = nullptr;
                    node->variableID = value.ID;
                }
                node->actual_type = value.type;
                if (!value.is_mutable) node->is_mut = false;
            }

            if (node->actual_type.type == NodeTypeType::Function) node->is_mut = false;
        }
        else if (node->value == "as") {
            type2 = type_to_item(type2);
            Check_as_Operator_Pair(type1, type2, "value type mismatch in as operator!");
            node->actual_type = type2;
            if (value1.has_value()) {
                if (type1 == type2 || is_number(type1)) node->const_value = value1;
                else if (type1 == Bool) node->const_value = static_cast<long long>(std::any_cast<bool>(value1));
                else if (type1 == Char) node->const_value = static_cast<long long>(std::any_cast<char>(value1));
            }
            node->variableID = ++Item_id_tot;
        }
    }
    else if (node->type == AstNodetype::Char_Literal) {
        node->actual_type = Char;
        if (node->value[0] == '\\') {
            if (node->value[1] == 'n') node->const_value = '\n';
            else if (node->value[1] == 't') node->const_value = '\t';
            else if (node->value[1] == 'r') node->const_value = '\r';
            else if (node->value[1] == '\\') node->const_value = '\\';
            else if (node->value[1] == '\'') node->const_value = '\'';
            else if (node->value[1] == '\"') node->const_value = '\"';
            else if (node->value[1] == '\0') node->const_value = '\0';
            else throw std::runtime_error("Semantic Error: unknown escape character in char literal!");
        }
        else node->const_value = node->value[0];
    }
    else if (node->type == AstNodetype::Integer_Literal) {
        auto str = node->value;
        if (str.size() > 3 && str.substr(str.size() - 3, 3) == "i32") {
            node->actual_type = I32;
            node->const_value = static_cast<long long>(std::stoi(str.substr(0, str.size() - 3), nullptr, 0));
        }
        else if (str.size() > 4 && str.substr(str.size() - 4, 3) == "_i32") {
            node->actual_type = I32;
            node->const_value = static_cast<long long>(std::stoi(str.substr(0, str.size() - 4), nullptr, 0));
        }
        else if (str.size() > 3 && str.substr(str.size() - 3, 3) == "u32") {
            node->actual_type = U32;
            node->const_value = static_cast<long long>(std::stoul(str.substr(0, str.size() - 3), nullptr, 0));
        }
        else if (str.size() > 4 && str.substr(str.size() - 4, 3) == "_u32") {
            node->actual_type = U32;
            node->const_value = static_cast<long long>(std::stoul(str.substr(0, str.size() - 4), nullptr, 0));
        }
        else if (str.size() > 5 && str.substr(str.size() - 5, 4) == "isize") {
            node->actual_type = Isize;
            node->const_value = std::stoll(str.substr(0, str.size() - 5), nullptr, 0);
        }
        else if (str.size() > 6 && str.substr(str.size() - 6, 4) == "_isize") {
            node->actual_type = Isize;
            node->const_value = std::stoll(str.substr(0, str.size() - 6), nullptr, 0);
        }
        else if (str.size() > 5 && str.substr(str.size() - 5, 4) == "usize") {
            node->actual_type = Usize;
            node->const_value = static_cast<long long>(std::stoull(str.substr(0, str.size() - 5), nullptr, 0));
        }
        else if (str.size() > 6 && str.substr(str.size() - 6, 4) == "_usize") {
            node->actual_type = Usize;
            node->const_value = static_cast<long long>(std::stoull(str.substr(0, str.size() - 6), nullptr, 0));
        }
        else {//I need to infer it by myself
            auto res = std::stoll(str, nullptr, 0);
            node->const_value = res;
            if (father->type == AstNodetype::Unary_Operator && father->value == "-") res = -res;
            if (res > UINT_MAX || res < INT_MIN) throw std::runtime_error("Semantic Error: invalid integer literal!");
            if (res >= 0 && res <= INT_MAX) node->actual_type = Int;
            else if (res < 0) node->actual_type = IInt;
            else node->actual_type = UInt;
        }
    }
    else if (node->type == AstNodetype::String_Literal) {
        node->actual_type = Str_Amp;
        node->const_value = node->value;
    }
    else if (node->type == AstNodetype::Float_Literal) {
        //yeah you are right, cause you are not you
    }
    else if (node->type == AstNodetype::Bool_Literal) {
        node->actual_type = Bool;
        node->const_value = node->value == "true";
    }
    else if (node->type == AstNodetype::Break) {
        node->actual_type = Never;
        if (loop_father == nullptr) throw std::runtime_error("Semantic Error: no loop father!");
        if (loop_father->type == AstNodetype::While) {
            if (loop_father->now_go_son_id == 0) throw std::runtime_error("Semantic Error: cannot break in while condition!");
            if (node->children.size()) throw std::runtime_error("Semantic Error: cannot return sth in while condition!");
        }
        else {//AstNodetype::Loop
            auto res_type = !node->children.empty() ? node->children.back()->actual_type : Unit;
            if (res_type != Never) {
                if (loop_father->actual_type == UnKnown) loop_father->actual_type = res_type;
                else Expect_Type_match(loop_father->actual_type, res_type, "break expression type mismatch!");
            }
        }
    }
    else if (node->type == AstNodetype::Continue) {
        node->actual_type = Never;
        if (loop_father == nullptr) throw std::runtime_error("Semantic Error: no loop father!");
    }
    else if (node->type == AstNodetype::Return) {
        node->actual_type = Never;
        auto res_type = Unit;
        if (function_father == nullptr) throw std::runtime_error("Semantic Error: no function father!");
        if (!node->children.empty()) res_type = node->children.back()->actual_type;
        auto match_type = type_to_item(function_father->children[1]->actual_type);
        Expect_Type_match(match_type, res_type, "Semantic Error: return expression type mismatch!");
    }
    else if (node->type == AstNodetype::FunctionCall) {
        auto function_type = node->children[0]->actual_type;
        if (function_type.type != NodeTypeType::Function) throw std::runtime_error("Semantic Error: function call but name is not a function!");
        if (function_type.items_type.size() != node->children[1]->children.size()) throw std::runtime_error("Semantic Error: function call but args number not match!");
        for (int i = 0; i < function_type.items_type.size(); i++) {
            auto type1 = *function_type.items_type[i];
            auto type2 = node->children[1]->children[i]->actual_type;
            Expect_Type_match(type1, type2, "Semantic Error: function call but args number type mismatch at item " + std::to_string(i) + "![0 index]");
        }
        node->actual_type = *function_type.inside_type;
        if (node->actual_type != Unit)
            node->variableID = ++Item_id_tot;
    }
    else if (node->type == AstNodetype::ArrayIndex) {
        auto type1 = node->children[0]->actual_type;
        auto type2 = node->children[1]->actual_type;
        auto value1 = node->children[0]->const_value;
        auto value2 = node->children[1]->const_value;
        node->is_mut = node->children[0]->is_mut;
        node->is_variable = node->children[0]->is_variable;
        while (type1.type == NodeTypeType::Mut_Amp || type1.type == NodeTypeType::Amp) {
            node->is_mut = type1.type == NodeTypeType::Mut_Amp;
            type1 = *type1.inside_type;
        }
        if (type1.type != NodeTypeType::Array || (type2 != Int && type2 != UInt && type2 != Usize)) throw std::runtime_error("Semantic Error: array index type mismatch!");
        node->actual_type = *type1.inside_type;
        if (value2.has_value()) {
            auto index_value = static_cast<unsigned int>(std::any_cast<long long>(value2));
            if (type1.item_length <= index_value) throw std::runtime_error("Semantic Error: array index value out of range");
            if (value1.has_value()) node->const_value = std::any_cast<std::vector<std::any>>(value1)[index_value];
        }
    }
    else if (node->type == AstNodetype::StructField) {
        auto Type = node->children[0]->actual_type;
        if (Type.type != NodeTypeType::Type_of_Type || Type.inside_type->type != NodeTypeType::Struct) throw std::runtime_error("Semantic Error: struct field type mismatch!");
        auto inside_type = Type.inside_type;
        std::vector<std::string> Name;
        for (auto child: node->children[1]->children) {
            auto name = node->value;
            auto type1 = inside_type->field->get_item("@" + name).type, type2 = child->children[0]->actual_type;
            Name.push_back(name);
            Expect_Type_match(type1, type2, "Semantic Error: struct field type mismatch!");
        }
        std::sort(Name.begin(), Name.end());
        Name.erase(std::unique(Name.begin(), Name.end()), Name.end());
        if (Name.size() != Type.items_type.size()) throw std::runtime_error("Semantic Error: struct field lost some item!");
        if (Name.size() != node->children[1]->children.size()) throw std::runtime_error("Semantic Error: struct field give two same item!");
        node->actual_type = *Type.inside_type;
    }
}

void build_universe_scope(AstNode* node) {
    node->scope_value = new Scope();
    node->scope_father = nullptr;

    Item_id_tot = 1;
    //id = 1 is for function .len

    NodeType funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.items_type.push_back(&I32); funct.is_exit = true;
    node->scope_value->add_item("exit", scope_item(funct, std::any(), false, true, ++Item_id_tot));
    funct.is_exit = false;

    node->scope_value->add_type("printInt", scope_item(funct, std::any(), false, true, ++Item_id_tot));
    node->scope_value->add_type("printlnInt", scope_item(funct, std::any(), false, true, ++Item_id_tot));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.items_type.push_back(&Str_Amp);
    node->scope_value->add_type("print", scope_item(funct, std::any(), false, true, ++Item_id_tot));
    node->scope_value->add_type("println", scope_item(funct, std::any(), false, true, ++Item_id_tot));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.inside_type = &I32;
    node->scope_value->add_type("getInt", scope_item(funct, std::any(), false, true, ++Item_id_tot));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.inside_type = &String;
    node->scope_value->add_type("getString", scope_item(funct, std::any(), false, true, ++Item_id_tot));
}

void Semantic_check(AstNode* node) {
    build_universe_scope(node);
    semantic_visit_node(node);
}