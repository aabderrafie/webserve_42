#include "server.hpp"

Server::Server(const Config& config) : config(config) {
    for (size_t i = 0; i < config.listen_ports.size(); ++i) {
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
            throw std::runtime_error("Error creating socket");

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(config.listen_ports[i]);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        memset(&(server_addr.sin_zero), 0, 8);

        struct pollfd server_poll_fd;
        server_poll_fd.fd = server_socket;
        server_poll_fd.events = POLLIN;
        poll_fds.push_back(server_poll_fd);

        server_sockets.push_back(server_socket);
        server_addrs.push_back(server_addr);
    }
}

Server::~Server() {
    std::cout << "Closing server sockets" << std::endl;
    for (int server_socket : server_sockets) {
        close(server_socket);
    }
}

void Server::bind_and_listen() {
    for (size_t i = 0; i < server_sockets.size(); ++i) {
        int opt = 1;
        if (setsockopt(server_sockets[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            throw std::runtime_error("Error setting socket options");

        if (bind(server_sockets[i], (struct sockaddr *)&server_addrs[i], sizeof(server_addrs[i])) < 0)
            throw std::runtime_error("Error binding to port " + std::to_string(ntohs(server_addrs[i].sin_port)));

        if (listen(server_sockets[i], BACKLOG) < 0)
            throw std::runtime_error("Error listening on port " + std::to_string(ntohs(server_addrs[i].sin_port)));
    }
}

void Server::new_connection(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sin_size);
    if (client_socket < 0)
        throw std::runtime_error("Error accepting connection");

    struct pollfd client_poll_fd;
    client_poll_fd.fd = client_socket;
    client_poll_fd.events = POLLIN;
    poll_fds.push_back(client_poll_fd);
}

void Server::handle_client(int client_socket) {
    Response response(client_socket, config);

    char buffer[BUFFER_SIZE];
    std::string body;
    int bytes_received;
    while (true) {
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0)
            throw std::runtime_error("Error receiving data");
        else if (bytes_received == 0)
            break;
        buffer[bytes_received] = '\0';
        body += std::string(buffer, bytes_received);
        if (bytes_received < BUFFER_SIZE - 1)
            break;
    }

    std::istringstream request(body);
    std::string method, uri, version;
    request >> method >> uri >> version;

    if (method == "GET")
        response.handle_get_request(uri, config);
    else if (method == "POST")
        response.handle_post_request(uri, body, config);
    else
        response.send_error_response(405, "text/html", config.root_location.root + config.error_pages[1].second);

    close(client_socket);
}

void Server::start_server() {
    bind_and_listen();

    std::cout << "Server is running on ports ";
    for (const auto& addr : server_addrs) {
        std::cout << ntohs(addr.sin_port) << " ";
    }
    std::cout << std::endl;

    while (true) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0)
            throw std::runtime_error("Error polling for events");

        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (std::find(server_sockets.begin(), server_sockets.end(), poll_fds[i].fd) != server_sockets.end())
                    new_connection(poll_fds[i].fd);
                else {
                    handle_client(poll_fds[i].fd);
                    poll_fds.erase(poll_fds.begin() + i);
                    --i;
                }
            }
        }
    }

    for (int server_socket : server_sockets) {
        close(server_socket);
    }
}