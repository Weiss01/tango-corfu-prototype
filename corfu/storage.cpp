#include "../sockets/mysocket.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#define MESSAGE_LENGTH 1024
#define ACTION_LENGTH 64

MySocket storage("Storage");
std::string port;
bool flag = 1;
char *fn_buffer;

void read_log(int client_fd);
void write_to_log(int client_fd);
bool isNumber(std::string s);
void start();

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Wrong Number of Arguments!" << std::endl;
        return 1;
    }
    
    port = std::string(argv[1]);
    
    if (!isNumber(port)) {
        std::cout << "Invalid Port Number!" << std::endl;
        return 1;
    }

    std::string file_name = "sharedlogs/" + port + ".txt";
    fn_buffer = (char *) malloc((file_name.length() + 1) * sizeof(*fn_buffer));
    strcpy(fn_buffer, file_name.c_str());

    start();

    return 0;
}

void start() {
    storage.bind("127.0.0.1", std::stoi(port));

    storage.listen();

    std::string action;

    while (flag) {
        int fd = storage.accept();
        action = MySocket::recv(fd, ACTION_LENGTH);
        if (action.compare("write") == 0) {
            MySocket::send(fd, "writing");
            write_to_log(fd);
        } else if (action.compare("read") == 0) {
            MySocket::send(fd, "reading");
            read_log(fd);
        }
    }
    
    storage.close();
    storage.shutdown();
}

void write_to_log(int client_fd) {
    std::string message = MySocket::recv(client_fd, MESSAGE_LENGTH);
    std::cout << "Storage " << storage.port_number << " writing " << message << std::endl;

    FILE *fp;
    fp = fopen(fn_buffer, "a");
    if (fp == NULL) {
        std::cout << "Error Writing to File" << std::endl;
        MySocket::send(client_fd, "fail");
        return;
    } else {
        fprintf(fp, "%s\n", message.c_str());
        fclose(fp);
    }
    MySocket::send(client_fd, "success");
}

void read_log(int client_fd) {
    std::string message = MySocket::recv(client_fd, MESSAGE_LENGTH);
    int index = std::stoi(message);

    std::string index_str = std::to_string(index);
    std::ifstream in(fn_buffer);

    std::string line;
    line.reserve(MESSAGE_LENGTH);

    std::string current_index;

    while (!in.eof()) {
        std::getline(in, line);
        std::stringstream ss(line);
        ss >> current_index;
        if (current_index.compare(index_str) == 0) {
            MySocket::send(client_fd, line);
            break;
        }
    }
    MySocket::send(client_fd, "NOT FOUND");
}

bool isNumber(std::string s) {
    return std::find_if(
        s.begin(), s.end(),
        [](unsigned char c) {
            return !std::isdigit(c);
        }
    ) == s.end();
}