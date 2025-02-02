#include "server.hpp"
#include <ctime>
#include <fcntl.h>  // For fcntl()
#include <unistd.h> // For close()

std::string current_time() {
    std::time_t now = std::time(0);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}
bool Server::is_server_socket(int fd) {
    return std::find(server_sockets.begin(), server_sockets.end(), fd) != server_sockets.end();
}

bool Server::is_client_socket(int fd) {
    return std::find_if(poll_fds.begin(), poll_fds.end(),
                        [fd](const struct pollfd& pfd) { return pfd.fd == fd; }) != poll_fds.end();
}

Server::Server() {}

void Server::server_init() {
    for (size_t i = 0; i < ports.size(); ++i) {
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
            throw std::runtime_error("Error creating socket");

        fcntl(server_socket, F_SETFL, O_NONBLOCK);

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(ports[i]);
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
    for (size_t i = 0; i < server_sockets.size(); ++i) {
        close(server_sockets[i]);
    }
}

void Server::bind_and_listen() {
    for (size_t i = 0; i < server_sockets.size(); ++i) {
        int opt = 1;
        if (setsockopt(server_sockets[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            throw std::runtime_error("Error setting socket options");

        if (bind(server_sockets[i], (struct sockaddr *)&server_addrs[i], sizeof(server_addrs[i])) < 0)
            throw std::runtime_error("Error binding to port " + std::to_string(ntohs(server_addrs[i].sin_port) ));

        if (listen(server_sockets[i], BACKLOG) < 0)
            throw std::runtime_error("Error listening on port " + std::to_string(ntohs(server_addrs[i].sin_port)));
    }
}

void Server::new_connection(int server_socket) {

    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(client_addr);
    
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sin_size);
    if (client_socket < 0) {
        if (errno != EWOULDBLOCK) 
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        return;
    }

    fcntl(client_socket, F_SETFL, O_NONBLOCK);

    struct pollfd client_poll_fd;
    client_poll_fd.fd = client_socket;
    client_poll_fd.events = POLLIN;
    poll_fds.push_back(client_poll_fd);

    std::cout << BLUE << "[" << current_time() << "] New client connected on socket " << client_socket << RESET << std::endl;
}

std::string Server::read_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    std::string body;
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        if (bytes_received < 0) {
            if (errno == EWOULDBLOCK) break; // No more data, exit loop
            throw std::runtime_error("Error receiving data");
        }
        buffer[bytes_received] = '\0';
        body += std::string(buffer, bytes_received);
        if (bytes_received < BUFFER_SIZE - 1) break; // Partial read, exit loop
    }

    return body;
}

void Server::handle_client(int client_socket) {
    std::cout << "Handling client" << std::endl;
    
    try {
        Response response(client_socket, *this);
        std::string body = read_request(client_socket);
        std::cout << "Body: " << body << std::endl;

        std::string method = "GET"; // Temporary (should be parsed from request)
        std::cout << YELLOW << "[" << current_time() << "] Request method: " << method << RESET << std::endl;

        if (method == "GET")
            response.handle_get_request(body);
        else if (method == "POST")
            response.handle_post_request(body);
        else if (method == "DELETE")
            response.handle_delete_request(body);
        else
            response.send_error_response(405, "text/html", root_location.root + error_pages[405]);

        close(client_socket);
    } catch (const std::exception& e) {
        std::cerr << "Error handling client: " << e.what() << std::endl;
        close(client_socket);
    }
}

void Server::start_server() {
    server_init();
    bind_and_listen();
    std::cout << GREEN << "[" << current_time() << "] Server is ready to accept connections." << RESET << std::endl;
}

/* 
void Server::start_server() {
    server_init();
    bind_and_listen();

    while (true) {
        std::cout << "Polling for events" << std::endl;
        std::cout << "Poll fds size: " << poll_fds.size() << std::endl; 
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0)

            throw std::runtime_error("Error polling for events");
        std::cout << "Poll count: " << poll_count << std::endl;
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
}*/