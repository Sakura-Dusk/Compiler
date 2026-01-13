//
// Created by Sakura on 26-1-12.
//

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include<climits>
#include"../lexer.h"
#include"../AST/node/basic.h"

void semantic_visit_node(AstNode*, AstNode*);

void build_universe_scope(AstNode*);

void Semantic_check(AstNode*);

#endif //SEMANTIC_H
