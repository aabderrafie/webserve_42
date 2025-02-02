#pragma once

#include "server.hpp"
class server;

class MultiServer {
    private:
        const Config& config;
        std::vector<Server> servers; 
        size_t _size;
    public:
        MultiServer(const Config& config);
        ~MultiServer();
        void start_servers();

};
