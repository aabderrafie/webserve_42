#include "MultiServer.hpp"

void ports_print(std::vector<int> ports){
    for(auto port : ports)
        std::cout << port << " ";
}
MultiServer::MultiServer(const Config& config) : config(config), _size(config.servers.size()) {
    for (size_t i = 0; i < _size; ++i) 
        servers.push_back(Server(config.servers[i]));

}

void MultiServer::start_servers(){
    std::cout << RED << "           ----- webserve 1337 -----         " << RESET << std::endl;
    
    for (size_t i = 0; i < _size; ++i) {
        std::cout << GREEN << "           ----- Server " << i << " is running -----         " << RESET <<  "with ports{";
        ports_print(servers[i].ports);
        std::cout << "}" << std::endl;
        // servers[i].start_server();
    }
    servers[0].start_server();
}

MultiServer::~MultiServer() {
//     for (size_t i = 0; i < _size; ++i) {
//         std::cout << RED << "           ----- Server " << i << " is closing -----         " << RESET << std::endl;
//         std::cout << "Closing server " << i << std::endl;
//     }
//     servers.clear();
 }