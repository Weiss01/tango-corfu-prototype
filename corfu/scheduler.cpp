#include "../sockets/mysocket.h"

#include <pthread.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <unordered_map>

#define UNUSED(x) (void)(x)

#define SCHEDULER_PORT 6969
#define LAYOUT_PORT 6970
#define VARIABLE_MESSAGE_LENGTH 512

pthread_t *threads;
pthread_t layout_thread;

int server_count;
std::string *ports;
int *cmd_args;
int counter = 0;
std::unordered_map<std::string, int> map;

void init_storage();
void *start_layout(void *arg);
void start_scheduler();
void *exec(void *arg);

int main() {
    init_storage();
    pthread_create(&layout_thread, NULL, start_layout, NULL);
    
    start_scheduler();

    int i;
    for (i = 0; i < server_count; i++)
        pthread_join(threads[i], NULL);
    pthread_join(layout_thread, NULL);

    free(cmd_args);
    return 0;
}

void start_scheduler() {
    MySocket scheduler("Scheduler", SCHEDULER_PORT);
    scheduler.listen();

    while (true) {
        int fd = scheduler.accept();

        std::string action = MySocket::recv(fd, VARIABLE_MESSAGE_LENGTH);
        MySocket::send(fd, "ack");
        if (action.compare("write") == 0) {
            std::string variables = MySocket::recv(fd, VARIABLE_MESSAGE_LENGTH);

            std::string var, var_num;
            std::stringstream ss(variables);
            std::string backpointers = "";
            std::string backpointer;

            ss >> var_num;
            for (int i = 0; i < std::stoi(var_num); i++) {
                ss >> var;
                if (map.find(var) == map.end())
                    backpointer = "-1";
                else
                    backpointer = std::to_string(map.at(var));
                backpointers += backpointer + " ";
                map[var] = counter;
                // TODO: variable might fail when writing, handle this
            }

            MySocket::send(fd, std::to_string(counter));
            MySocket::recv(fd, 64); // prevents blocking
            MySocket::send(fd, backpointers);

            counter += 1;
        } else if (action.compare("read") == 0) {
            std::string variable = MySocket::recv(fd, VARIABLE_MESSAGE_LENGTH);
            int true_pointer;
            if (map.find(variable) == map.end())
                true_pointer = -1;
            else
                true_pointer = map.at(variable);
            MySocket::send(fd, std::to_string(true_pointer));
        }
    }
    
    scheduler.close();
    scheduler.shutdown();
}

void *start_layout(void *arg) {
    UNUSED(arg);
    std::string layout_config = std::to_string(server_count);
    layout_config += " ";

    int i;
    for (i = 0; i < server_count; i++) {
        layout_config += ports[i];
        layout_config += " ";
    }

    MySocket layout("Layout", LAYOUT_PORT);
    layout.listen();

    while (true) {
        int fd = layout.accept();
        std::cout << "Client connected - Sending Port Configurations" << std::endl;
        MySocket::send(fd, layout_config);
    }

    return NULL;
}

void init_storage() {
    FILE *fp = fopen("corfu/corfu.config", "r");
    if (fp == NULL) {
        std::cout << "Failed to Read Configuration File. Please Verify File Path!" << std::endl;
        exit(1);
    }

    char buffer[150];

    fgets(buffer, sizeof(buffer), fp);
    server_count = std::stoi(std::string(buffer));
    std::cout << "Number of Storage Servers: " << server_count << std::endl;
    ports = (std::string *) malloc(server_count * sizeof(*ports));
    threads = (pthread_t *) malloc(server_count * sizeof(*threads));
    cmd_args = (int *) malloc(server_count * sizeof(*cmd_args));

    fgets(buffer, sizeof(buffer), fp);
    std::string str = std::string(buffer);
    std::stringstream ss(str);

    std::string port;
    int i;
    for (i = 0; i < server_count; i++) {
        ss >> port;
        std::cout << "Spinning up Storage @Port" << port << std::endl;
        ports[i] = port; cmd_args[i] = std::stoi(port);
        pthread_create(&threads[i], NULL, exec, &cmd_args[i]);
    }
    fclose(fp);
}

void *exec(void *arg) {
    int cmd_port = *(int *) arg;
    
    std::string cmd_str = "./storage " + std::to_string(cmd_port);
    
    char cmd[cmd_str.length() + 1];
    strcpy(cmd, cmd_str.c_str());

    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    char buffer[128];
    try {
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
            std::cout << buffer << std::endl;
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return NULL;
}