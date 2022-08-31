#include <string>

class Node {
    public:
        std::string key;
        int value;
        Node *nxt;
        Node *prev;

        Node(std::string key, int value);
        ~Node();
};