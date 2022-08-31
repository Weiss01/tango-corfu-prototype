#include <memory>
#include <iostream>
#include <sstream>
#include <utility>
#include <unordered_map>
#include <vector>
#include <string>

class Client {
    public:
        static const int LAYOUT_LENGTH, RECORD_LENGTH, MESSAGE_LENGTH, SCHEDULER_PORT, LAYOUT_PORT;

        int server_count;
        std::string *ports;
        std::unordered_map<std::string, int> local_map;

        Client();
        void update(std::string variable, int true_pointer, void (*parse)(std::vector<std::string>, std::string));
        void update_multiple(std::vector<std::string> variables, void (*parse)(std::vector<std::string>, std::string));
        void write(std::vector<std::pair<std::string, std::string>> operations, void (*parse)(std::vector<std::string>, std::string));
};