#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

class MySocket {
    public:
        std::string address;
        std::string name;
        int port_number;
        int fd;
        struct sockaddr_in sock_address;
        socklen_t sock_address_length;

        MySocket(std::string name);
        MySocket(std::string name, int port_number);
        MySocket(std::string name, std::string address, int port_number);
        void bind(std::string address, int port_number);
        void listen();
        int accept();
        int connect(std::string ip_address, int port_number);
        void close();
        void shutdown();
        
        static std::string recv(int fd, int length);
        static void send(int fd, std::string message);

    private:
        int init_socket_fd();
        struct sockaddr_in get_socket_addr();
        struct sockaddr_in get_destination_addr(std::string ip_address, int port_number);
};