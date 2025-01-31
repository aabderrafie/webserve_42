#pragma once
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
#include <poll.h>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <sys/socket.h>
#include <ctime>
#include <iomanip>
#include <cctype>

#define PORT 80
#define BACKLOG 10
#define BUFFER_SIZE 1024
#define clinet_max 1000000000

#include "Response.hpp"
#include "config.hpp"
#include "Request.hpp"

#include <ctime>
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
std::string current_time();



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