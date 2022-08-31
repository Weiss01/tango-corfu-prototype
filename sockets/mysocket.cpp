#include "mysocket.h"

#include <iostream>

MySocket::MySocket(std::string name) {
    this->name = name;
    this->fd = init_socket_fd();
}

MySocket::MySocket(std::string name, int port_number) {
    this->name = name;
    this->fd = init_socket_fd();
    std::string address = "127.0.0.1";
    bind(address, port_number);
}

MySocket::MySocket(std::string name, std::string address, int port_number) {
    this->name = name;
    this->fd = init_socket_fd();
    bind(address, port_number);
}

void MySocket::bind(std::string address, int port_number) {
    this->address = address;
    this->port_number = port_number;
    this->sock_address = get_socket_addr();
    this->sock_address_length = sizeof(this->sock_address);
}

void MySocket::listen() {
    int flag = ::listen(this->fd, 3);
    if (flag == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "[LISTENING] " << name << " is listening at " << this->address << ":" << this->port_number << std::endl;
}

int MySocket::accept() {
    int fd;
    fd = ::accept(this->fd, NULL, NULL);
    if (fd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int MySocket::connect(std::string ip_address, int port_number) {
    struct sockaddr_in dest_address; 
    
    dest_address = get_destination_addr(ip_address, port_number);
    socklen_t address_length = sizeof(dest_address);

    int server = ::connect(this->fd, (struct sockaddr *) &dest_address, address_length);
    if (server == -1) {
        std::cout << this->name;
        printf(" Connection Failed \n");
        return -1;
    }
    return server;
}

void MySocket::close() {
    ::close(this->fd);
}

void MySocket::shutdown() {
    ::shutdown(this->fd, SHUT_RDWR);
}

std::string MySocket::recv(int fd, int length) {
    char *buffer;
    std::string message;

    buffer = (char *) malloc(length * sizeof(*buffer));
    read(fd, buffer, length);
    
    message = std::string(buffer);
    free(buffer);

    return message;
}

void MySocket::send(int fd, std::string message) {
    char *buffer = (char *) malloc((message.length() + 1) * sizeof(*buffer));
    strcpy(buffer, message.c_str());
    
    write(fd, buffer, (message.length() + 1));
    free(buffer);
}

int MySocket::init_socket_fd() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return fd;
}

struct sockaddr_in MySocket::get_socket_addr() {
    int flag, opt = 1;
    flag = setsockopt(
        this->fd, SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt)
    );

    if (flag == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->port_number);

    flag = ::bind(fd, (struct sockaddr *) &address, sizeof(address));
    if (flag == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return address;
}

struct sockaddr_in MySocket::get_destination_addr(std::string ip_address, int port_number) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port_number);

    char *buffer = (char *) malloc(ip_address.length() * sizeof(*buffer));
    strcpy(buffer, ip_address.c_str());

    if (inet_pton(AF_INET, buffer, &address.sin_addr) <= 0) {
        printf("\nInvalid address or Address not supported\n");
        exit(EXIT_FAILURE);
    }
    free(buffer);
    return address;
}