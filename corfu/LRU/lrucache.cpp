#include "lrucache.h"

#include <iostream>

LRUCache::LRUCache(int capacity) {
    Node *node = (Node *) malloc(sizeof(*node));
    node = new Node("null", -1);
    this->capacity = capacity;
    this->tail = node;
    this->head = this->tail;
}

void LRUCache::remove_tail() {
    Node *node = (Node *) malloc(sizeof(*node));
    node = (*(this->tail)).nxt;
    if ((*node).nxt != NULL) {
        Node *new_nxt = (Node *) malloc(sizeof(*new_nxt));
        new_nxt = (*node).nxt;
        (*(this->tail)).nxt = new_nxt;
        (*new_nxt).prev = this->tail;
    } else {
        this->head = this->tail;
    }
    this->table.erase((*node).key); 
}

Node *LRUCache::remove(std::string key) {
    Node *node = (Node *) malloc(sizeof(*node));
    node = this->table.at(key);
    if ((*node).nxt != NULL) {
        Node *prev = (Node *) malloc(sizeof(*prev));
        Node *nxt = (Node *) malloc(sizeof(*nxt));
        prev = (*node).prev;
        nxt = (*node).nxt;
        (*prev).nxt = nxt;
        (*nxt).prev = prev;
    } else {
        this->head = (*node).prev;
    }
    (*node).prev = NULL;
    (*node).nxt = NULL;
    return node;
}

void LRUCache::add_to_front(Node *node) {
    (*(this->head)).nxt = node;
    (*node).prev = this->head;
    this->head = (*(this->head)).nxt;
}

int LRUCache::get(std::string key) {
    bool key_exists = true;
    if (this->table.find(key) == this->table.end())
        key_exists = false;

    if (key_exists) {
        Node *node = (Node *) malloc(sizeof(*node));
        node = remove(key);
        add_to_front(node);
        return (*node).value;
    }
    return -1;
}

void LRUCache::put(std::string key, int value) {
    bool key_exists = true;
    if (this->table.find(key) == this->table.end())
        key_exists = false;

    if (key_exists) {
        Node *node = (Node *) malloc(sizeof(*node));
        node = remove(key);
        (*node).value = value;
        add_to_front(node);
    } else {
        if ((int) this->table.size() >= this->capacity)
            remove_tail();
        
        Node *node = (Node *) malloc(sizeof(*node));
        node = new Node(key, value);
        this->table[key] = node;
        add_to_front(node);
    }
}

void LRUCache::display() {
    std::string key;
    int value;
    int count = 0;

    std::cout << "=================================" << std::endl;
    for(auto it = this->table.begin(); it != this->table.end(); it++) {
        key = it->first; value = (*(it->second)).value;
        std::cout << "| " << key;
        for(int i = 0; i < (int) (14-key.length()); i++)
            std::cout << " ";
        std::cout << "| " << value;
        for(int i = 0; i < (int) (14-std::to_string(value).length()); i++)
            std::cout << " ";
        std::cout << "|" << std::endl;
        if (count != (int) this->table.size() - 1)
            std::cout << "---------------------------------" << std::endl;
        count += 1;
    }
    std::cout << "=================================" << std::endl << std::endl;

    Node *node = this->tail;

    while ((*node).nxt != NULL) {
        std::cout << (*((*node).nxt)).key << " --> ";
        node = (*node).nxt;
    }
    std::cout << "NULL" << std::endl << std::endl;
}