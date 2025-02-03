#pragma once

#include <vector>
#include <poll.h>
#include "config.hpp"
#include "server.hpp"

class MultiServer {
private:
    const Config& config;
    std::vector<Server> servers; 
    std::vector<struct pollfd> fds;
    size_t _size;
    
public:
    MultiServer(const Config& config);
    ~MultiServer();
    void start_servers();
    void initialize_servers();
    void setup_poll_fds();
};