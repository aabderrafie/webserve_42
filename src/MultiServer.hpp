#pragma once

#include "server.hpp"
#include <vector>
#include <stdexcept>

class Server; 

class MultiServer {
    private:
        const Config& config;
        std::vector<Server> servers; 
        std::vector<struct pollfd> poll_fds; 
        size_t _size;
        
    public:
        MultiServer(const Config& config);
        ~MultiServer();
        void start_servers();
};
