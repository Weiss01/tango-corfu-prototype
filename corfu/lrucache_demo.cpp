#include "client.h"
#include "LRU/lrucache.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

Client obj;
bool get_action = false;
LRUCache cache_a(3);
LRUCache cache_b(3);

void parse(std::vector<std::string> backlog, std::string variable);

int main() {
    std::string variable, operation;

    while (true) {
        std::vector<std::pair<std::string, std::string>> operations;
        printf("Cache: ");
        std::cin >> variable;
        if (variable.compare("!q") == 0)
            break;
        printf("Operation: ");
        std::cin >> operation;
        if (operation.compare("fresh") == 0) {
            get_action = false;
            std::vector<std::string> variables;
            variables.push_back("a");
            variables.push_back("b");
            obj.update_multiple(variables, parse);
        } else {
            get_action = true;
            operations.push_back(std::make_pair(variable, operation));
            obj.write(operations, parse);
        }
    }

    return 0;
}

void parse(std::vector<std::string> backlog, std::string variable) {
    bool is_retrieval = false;
    int result = -69;
    for (auto op = backlog.rbegin(); op != backlog.rend(); op++) {
        std::string log, operation, key;
        log = *op;
        operation = log.substr(0, 3);
        if (operation.compare("get") == 0) {
            if (op == backlog.rend()-1) is_retrieval = true;
            key = log.substr(log.find("(") + 1, log.find(")") - log.find("(") - 1);
            if (variable.compare("a") == 0) {
                result = cache_a.get(key);
            } else if (variable.compare("b") == 0) {
                result = cache_b.get(key);
            } else {
                std::cout << "Invalid Cache Variable" << std::endl;
            }
        } else if (operation.compare("put") == 0) {
            key = log.substr(log.find("(") + 1, log.find(",") - log.find("(") - 1);
            std::string value_str = log.substr(log.find(",") + 1, log.find(")") - log.find(",") - 1);
            int value = std::stoi(value_str);
            if (variable.compare("a") == 0) {
                cache_a.put(key, value);
            } else if (variable.compare("b") == 0) {
                cache_b.put(key, value);
            } else {
                std::cout << "Invalid Cache Variable" << std::endl;
            }
        } else {
            std::cout << "Invalid Operation" << std::endl;
        }
    }
    #if defined _WIN32
        system("cls");
    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
        system("clear");
    #elif defined (__APPLE__)
        system("clear");
    #endif
    
    std::cout << "CACHE_A" << std::endl;
    cache_a.display();
    std::cout << std::endl << "CACHE_B" << std::endl;
    cache_b.display();
    if (is_retrieval && get_action)
        std::cout << "Cache Retrieval = " << result << std::endl;
}