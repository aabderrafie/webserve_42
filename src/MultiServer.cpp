#include "MultiServer.hpp"

void ports_print(std::vector<int> ports) {
    for (auto port : ports)
        std::cout << port << " ";
}

MultiServer::MultiServer(const Config& config) : config(config), _size(config.servers.size()) {
    for (size_t i = 0; i < _size; ++i) 
        servers.push_back(Server(config.servers[i]));
}

void MultiServer::start_servers() {
    std::cout << RED << "           ----- webserve 1337 -----         " << RESET << std::endl;

    for (size_t i = 0; i < _size; ++i) {
        servers[i].start_server();  
        std::cout << GREEN << "           ----- Server " << i << " is running -----         " << RESET << " with ports {";
        ports_print(servers[i].ports);
        std::cout << "}" << std::endl;
        poll_fds.insert(poll_fds.end(), servers[i].poll_fds.begin(), servers[i].poll_fds.end());
    }


    while (true) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0)
            throw std::runtime_error("Error polling for events");
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                bool is_server_socket = false;

                for (size_t j = 0; j < _size; ++j) {
                    if (servers[j].is_server_socket(poll_fds[i].fd)) {
                        servers[j].new_connection(poll_fds[i].fd);
                        is_server_socket = true;
                        break;
                    }
                }

                if (!is_server_socket) {
                    for (size_t j = 0; j < _size; ++j) {
                        if (servers[j].is_client_socket(poll_fds[i].fd)) {
                            servers[j].handle_client(poll_fds[i].fd);
                            close(poll_fds[i].fd);
                            poll_fds.erase(poll_fds.begin() + i);
                            --i;  
                            break;
                        }
                    }
                }
            }
        }
    }
}

MultiServer::~MultiServer() {
    for (size_t i = 0; i < _size; ++i) {
        std::cout << RED << "           ----- Server " << i << " is closing -----         " << RESET << std::endl;
        std::cout << "Closing server " << i << std::endl;
    }
    servers.clear();
}
