#pragma once

#include "server.hpp"
#include <vector>
#include <stdexcept>

class Server; // Forward declaration to prevent cyclic dependency

class MultiServer {
    private:
        const Config& config;
        std::vector<Server> servers; 
        std::vector<struct pollfd> poll_fds; // Global poll list
        size_t _size;
        
    public:
        MultiServer(const Config& config);
        ~MultiServer();
        void start_servers();
};
