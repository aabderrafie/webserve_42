#include "MultiServer.hpp"
#include "colors.hpp"

void ports_tostring(const std::vector<int>& ports, std::string& ports_str) {
    for (size_t i = 0; i < ports.size(); ++i) {
        ports_str += std::to_string(ports[i]);
        if (i < ports.size() - 1)
            ports_str += ", ";
    }
}
MultiServer::MultiServer(const Config& config) : config(config), _size(config.servers.size()) {
    for (size_t i = 0; i < _size; ++i) 
        servers.push_back(Server(config.servers[i]));

}

void MultiServer::start_servers() {
    Draw::drawBox("SERVER INITIALIZATION");

    size_t server_count = 1;
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        std::string ports_str;
        ports_tostring((*it).ports, ports_str);
        
        it->server_init();
        it->bind_and_listen();
        Draw::serverStart(server_count++, ports_str);
    }

    Draw::drawBox("SERVERS NOW RUNNING");

    for (std::vector<Server>::iterator it = servers.begin(); true; ++it) {
        if (it == servers.end())
            it = servers.begin();
        try {
            (*it).start_server();
        } catch (const std::exception& e) {
            Draw::error(e.what());
        }
    }
}

MultiServer::~MultiServer() {
    Draw::drawBox("SHUTTING DOWN SERVERS");
    
    int server_count = 1;
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        Draw::serverClose(server_count++);
        for(size_t i = 0; i < it->server_sockets.size(); ++i) {
            close(it->server_sockets[i]);
        }
    }
    Draw::success("All servers shut down successfully");
}