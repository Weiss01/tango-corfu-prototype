#include "client.h"
#include "../sockets/mysocket.h"

#define LAYOUT_LENGTH 1024
#define RECORD_LENGTH 1024
#define MESSAGE_LENGTH 64
#define SCHEDULER_PORT 6969
#define LAYOUT_PORT 6970

Client::Client() {
    MySocket client("Client");
    client.connect("127.0.0.1", LAYOUT_PORT);
    std::string layout_config = MySocket::recv(client.fd, LAYOUT_LENGTH);
    client.close();

    std::string server_count_str, port;
    std::stringstream ss(layout_config);

    ss >> server_count_str;
    this->server_count = std::stoi(server_count_str);
    this->ports = (std::string *) malloc(this->server_count * sizeof(*(this->ports)));

    int i;
    for (i = 0; i < this->server_count; i++) {
        ss >> port;
        this->ports[i] = port;
    }
}

void Client::update(std::string variable, int true_pointer, void (*parse)(std::vector<std::string>, std::string)) {
    int local_pointer;
    if (this->local_map.find(variable) == this->local_map.end())
        local_pointer = -1;
    else local_pointer = this->local_map.at(variable);
    
    int port;
    std::string record, token, num_var, current_index = std::to_string(true_pointer);
    std::vector<std::string> backlog;

    while (std::stoi(current_index) != local_pointer) {
        port = std::stoi(this->ports[std::stoi(current_index) % this->server_count]);

        MySocket reader("Reader");
        reader.connect("127.0.0.1", port);
        MySocket::send(reader.fd, "read");
        MySocket::recv(reader.fd, MESSAGE_LENGTH); // prevents blocking
        MySocket::send(reader.fd, current_index);
        record = MySocket::recv(reader.fd, RECORD_LENGTH);
        reader.close();
        
        if (record.compare("NOT FOUND") == 0) {
            std::cout << "SOMETHING WENT WRONG!" << std::endl;
            break;
        }

        std::stringstream ss(record);
        ss >> token; ss >> num_var;
        int i, count = 0;
        for (i = 0; i < std::stoi(num_var); i++) {
            std::string var, operation;
            ss >> var; ss >> operation;
            if (var.compare(variable) == 0) {
                count = i;
                backlog.push_back(operation);
            }
        }

        for (i = 0; i < count+1; i++)
            ss >> token;
        current_index = token;
    }
    parse(backlog, variable);
    this->local_map[variable] = true_pointer;
}

void Client::update_multiple(std::vector<std::string> variables, void (*parse)(std::vector<std::string>, std::string)) {
    std::vector<std::pair<std::string, int>> true_pointers;

    for (auto var = variables.begin(); var != variables.end(); var++) {
        MySocket client("Updater");
        client.connect("127.0.0.1", SCHEDULER_PORT);
        MySocket::send(client.fd, "read");
        MySocket::recv(client.fd, MESSAGE_LENGTH); // prevents blocking
        MySocket::send(client.fd, *var);
        std::string tp_str = MySocket::recv(client.fd, MESSAGE_LENGTH); // prevents blocking
        true_pointers.push_back(std::make_pair(*var, std::stoi(tp_str)));
        client.close();
    }

    for (auto var = true_pointers.begin(); var != true_pointers.end(); var++)
        update((*var).first, (*var).second, parse);
}

void Client::write(std::vector<std::pair<std::string, std::string>> operations, void (*parse)(std::vector<std::string>, std::string)) {
    std::string variables = std::to_string(operations.size());
    variables += " ";
    for (auto kv = operations.begin(); kv != operations.end(); kv++)
        variables += (*kv).first + " ";

    int retry_count = 5;
    while (retry_count > 0) {
        MySocket client("Client");
        client.connect("127.0.0.1", SCHEDULER_PORT);
        MySocket::send(client.fd, "write");
        MySocket::recv(client.fd, MESSAGE_LENGTH); // prevents blocking
        MySocket::send(client.fd, variables);
        std::string storage_index = MySocket::recv(client.fd, MESSAGE_LENGTH);
        MySocket::send(client.fd, "ack");
        std::string backpointers = MySocket::recv(client.fd, LAYOUT_LENGTH);
        client.close();

        int port = std::stoi(this->ports[std::stoi(storage_index) % this->server_count]);

        std::string message = storage_index + " " + std::to_string(operations.size()) + " ";

        for (auto kv = operations.begin(); kv != operations.end(); kv++)
            message += (*kv).first + " " + (*kv).second + " ";

        message += backpointers;

        MySocket writer("Writer");
        writer.connect("127.0.0.1", port);
        MySocket::send(writer.fd, "write");
        MySocket::recv(writer.fd, MESSAGE_LENGTH); // prevents blocking
        MySocket::send(writer.fd, message);
        std::string status = MySocket::recv(writer.fd, MESSAGE_LENGTH);
        writer.close();
        if (status.compare("success") == 0) {
            std::stringstream ss(backpointers);
            std::string backpointer;
            for (auto kv = operations.begin(); kv != operations.end(); kv++) {
                ss >> backpointer;
                int local_latest;
                if (this->local_map.find((*kv).first) == this->local_map.end())
                    local_latest = -1;
                else
                    local_latest = this->local_map.at((*kv).first);
                if (std::stoi(backpointer) == local_latest) {
                    this->local_map[(*kv).first] = std::stoi(storage_index);
                    std::vector<std::string> operations_to_execute;
                    operations_to_execute.push_back((*kv).second);
                    parse(operations_to_execute, (*kv).first);
                } else {
                    std::vector<std::string> variable_to_update;
                    variable_to_update.push_back((*kv).first);
                    update_multiple(variable_to_update, parse);
                }
            }
            break;
        }
        std::cout << "Write Failed. Retrying..." << std::endl;
        retry_count -= 1;
    }
}