//
// Created by Sakura on 26-1-12.
//

#include "semantic.h"

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

            if (now_node->type == AstNodetype::Type || now_node->type == AstNodetype::IDENTIFIER) {
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
    	switch(son->type) {
        	//TODO: add other item type semantic analysis
            default:
                break;
        }
    }
}

void semantic_visit_node(AstNode* node, AstNode* father = nullptr) {
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
}

void build_universe_scope(AstNode* node) {
    node->scope_value = new Scope();
    node->scope_father = nullptr;

    NodeType funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.items_type.push_back(&I32);
    node->scope_value->add_item("exit", scope_item(funct, std::any(), false, true));
    node->scope_value->add_type("printInt", scope_item(funct, std::any(), false, true));
    node->scope_value->add_type("printlnInt", scope_item(funct, std::any(), false, true));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.items_type.push_back(&Str_Amp);
    node->scope_value->add_type("print", scope_item(funct, std::any(), false, true));
    node->scope_value->add_type("println", scope_item(funct, std::any(), false, true));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.inside_type = &I32;
    node->scope_value->add_type("getInt", scope_item(funct, std::any(), false, true));

    funct = NodeType(NodeTypeType::Function, &Unit, 0);
    funct.inside_type = &String;
    node->scope_value->add_type("getInt", scope_item(funct, std::any(), false, true));
}

void Semantic_check(AstNode* node) {
    build_universe_scope(node);
    semantic_visit_node(node);
}