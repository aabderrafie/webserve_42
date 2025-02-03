#include "MultiServer.hpp"
MultiServer::~MultiServer() {}
bool compare_pollfd(const pollfd& lhs, const pollfd& rhs) {
    return lhs.fd == rhs.fd && lhs.events == rhs.events;
}


void ports_print(std::vector<int> ports){
    for(auto port : ports)
        std::cout << port << " ";
}
MultiServer::MultiServer(const Config& config) : config(config), _size(config.servers.size()) {
    for (size_t i = 0; i < _size; ++i) 
        servers.push_back(Server(config.servers[i]));

}

#include <thread>

void MultiServer::start_servers() {
    std::thread server1_thread(&Server::start_server, &servers[0]);
    std::thread server2_thread(&Server::start_server, &servers[1]);

    server1_thread.join();
    server2_thread.join();
}

// void MultiServer::start_servers(){
//     std::cout << RED << "           ----- webserve 1337 -----         " << RESET << std::endl;
    
//     for (size_t i = 0; i < _size; ++i) {
//         std::cout << GREEN << "           ----- Server " << i << " is running -----         " << RESET <<  "with ports{";
//         ports_print(servers[i].ports);
//         std::cout << "}" << std::endl;
//     }
//     for(size_t i = 0; i < _size; i++)
//         servers[i].start_server();

//     for(size_t i =0; i < _size ;++i){
//         for (size_t j = 0; j < servers[i].poll_fds.size(); ++j)
//             fds.push_back(servers[i].poll_fds[j]);
//     }


//     while (true) {
//         poll(fds.data(), fds.size(), -1);

//         for (size_t i = 0; i < fds.size(); ++i) {
//             for (size_t j = 0; j < servers.size(); ++j) {
//                 if (fds[i].revents & POLLIN) {
//                     std::cout << "Debug: 111111" << std::endl;

//                     if (i < servers.size() && (std::find(servers[i].server_sockets.begin(), servers[i].server_sockets.end(), fds[i].fd) != servers[i].server_sockets.end())) {
//                         std::cout << "Debug: 2222222" << std::endl;
//                         servers[i].new_connection(fds[i].fd);
//                         std::cout << "Debug: 333333" << std::endl;
//                     } else {
//                         std::cout << "Debug: 444444" << std::endl;
//                         if (i < servers.size()) {
//                             servers[i].handle_client(fds[i].fd);
//                         }
//                         std::cout << "Debug: 555555" << std::endl;
//                         fds.erase(fds.begin() + i);
//                         --i;
//                         break; 
//                     }
//                 }
//             }
//         }
//     }

// }




