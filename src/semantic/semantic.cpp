//
// Created by Sakura on 26-1-12.
//

#include "semantic.h"

#include <complex>

#include "../../cmake-build-release/_deps/googletest-src/googletest/include/gtest/internal/gtest-port.h"

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
    if (a.type != NodeTypeType::Type_of_Type) throw std::runtime_error("Semantic Error: expected type type!");
    return *a.inside_type;
}

//just go backwards
NodeType item_to_type(NodeType* a) {
 	if (*a == Unit) return *a;
	NodeType T(NodeTypeType::Type_of_Type, a, 0);
	return T;
}

NodeType item_to_type(NodeType &a) {
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
    auto res = scope_value->get_type(name);
    if (res.type != NodeTypeType::Unknown) return res;
    if (scope_father == nullptr) return res;
    return find_scope_type(scope_father->scope_value, scope_father->scope_father, name);
}

scope_item& find_scope_item(Scope* scope_value, const AstNode* scope_father, const std::string& name) {
    auto res = scope_value->get_item(name);
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
            semantic_visit_node(son->children[0], nullptr);
            son->scope_value->item_table.erase("self");

            //FunctionReturnType
            son->children[1]->scope_value = son->scope_value;
            son->children[1]->scope_father = son->scope_father;
            semantic_visit_node(son->children[1], nullptr);

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
                    if (to->type == AstNodetype::Amp) has_ref = true;
                    if (to->type == AstNodetype::Mut) has_mut = true;
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
                semantic_visit_node(son->children[0], nullptr);
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
            semantic_visit_node(son->children[0], nullptr);

            son->children[1]->scope_value = son->scope_value;
            son->children[1]->scope_father = son->scope_father;
            semantic_visit_node(son->children[1], nullptr);

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
            semantic_visit_node(son->children[0], nullptr);
        }
    }
}

void semantic_visit_node(AstNode* node, AstNode* father = nullptr, AstNode* loop_father = nullptr, AstNode* function_father = nullptr) {
    //TODO: implement semantic analysis visit
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
        if (find_scope_item(node->scope_value, node->scope_father, node->value).is_uncoverable) throw std::runtime_error("Semantic Error: redefinition of a uncoverable variable " + node->value + "!");
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

    }
}

void build_universe_scope(AstNode* node) {
    node->scope_value = new Scope();
    node->scope_father = nullptr;

    Item_id_tot = 0;

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