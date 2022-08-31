#include "node.h"

Node::Node(std::string key, int value) {
    this->key = key;
    this->value = value;
    this->nxt = NULL;
    this->prev = NULL;
}

Node::~Node() {
    free(this);
}