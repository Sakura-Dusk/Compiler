//
// Created by Sakura on 25-9-22.
//

#include "basic.h"

AstNode::~AstNode() {
  for (auto child : children) {
    delete child;
  }
  children.clear();
}