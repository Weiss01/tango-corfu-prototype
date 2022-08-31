#include "node.h"

#include <unordered_map>
#include <string>

class LRUCache {
    public:
        int capacity;
        std::unordered_map<std::string, Node*> table;
        Node *tail;
        Node *head;

        LRUCache(int capacity);
        void remove_tail();
        Node *remove(std::string key);
        void add_to_front(Node *node);
        int get(std::string key);
        void put(std::string key, int value);
        void display();
};