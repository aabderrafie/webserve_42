#pragma once
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <string>
#include <sys/stat.h>
#include <unistd.h>  
#include "Response.hpp"
#include "config.hpp"
#include "Request.hpp"
#include <poll.h>
#include <vector>
#include <algorithm>
#define PORT 80
#define BACKLOG 10
#include <fcntl.h>
#define BUFFER_SIZE 1024
#define clinet_max 1000000000

class Server {
    private:
        const Config& config;
        std::vector<int> server_sockets;
        std::vector<struct sockaddr_in> server_addrs;
        std::vector<struct pollfd> poll_fds;

    public:
        Server(const Config& config);
        ~Server();
        void start_server();
        void new_connection(int server_socket);
        void bind_and_listen();
        void handle_client(int client_socket);
};