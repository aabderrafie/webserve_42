#include "server.hpp"
#include <ctime>



std::string current_time() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

Server::Server()  {}

void Server::server_init() {
    for (size_t i = 0; i < ports.size(); ++i) {
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
            throw std::runtime_error("Error creating socket");
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
    // std::cout << RED << "[" << current_time() << "] Server shutting down..." << RESET << std::endl;
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


size_t Server::get_content_length(const std::string& headers) {
    size_t content_length_pos = headers.find("Content-Length: ");
    if (content_length_pos == std::string::npos) {
        throw std::runtime_error("Content-Length header not found");
    }

    size_t content_length_end = headers.find("\r\n", content_length_pos);
    if (content_length_end == std::string::npos) {
        throw std::runtime_error("Malformed Content-Length header");
    }

    std::string content_length_str = headers.substr(content_length_pos + 16, content_length_end - (content_length_pos + 16));
    return std::stoul(content_length_str);  // Convert string to size_t
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
    std::cout << BLUE << "[" << current_time() << "] New client connected on socket " << client_socket << RESET << std::endl;
}


std::unordered_map<int, std::string> partial_requests;

std::string Server::read_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

    if (bytes_received < 0)
            return "";

    if (bytes_received == 0) {
        std::string final_request = partial_requests[client_socket];
        partial_requests.erase(client_socket);
        return final_request;
    }

    buffer[bytes_received] = '\0';
    partial_requests[client_socket] += std::string(buffer, bytes_received);

    size_t header_end = partial_requests[client_socket].find("\r\n\r\n");
    if (header_end == std::string::npos) 
        return ""; 

    std::string headers = partial_requests[client_socket].substr(0, header_end + 4);
    size_t content_length_pos = headers.find("Content-Length: ");
    if (content_length_pos != std::string::npos) {
        size_t content_length_end = headers.find("\r\n", content_length_pos);
        int content_length = std::stoi(headers.substr(content_length_pos + 16, content_length_end - content_length_pos - 16));
        size_t total_length = header_end + 4 + content_length;
        if (partial_requests[client_socket].size() < total_length)
            return "";
    }
    std::string full_request = partial_requests[client_socket];
    partial_requests.erase(client_socket);
    std::cout << "Full request received" << std::endl;
    return full_request;
}

bool Server::check_method(const std::string& method, const std::vector<std::string>& allowed_methods) {
    return std::find(allowed_methods.begin(), allowed_methods.end(), method) != allowed_methods.end();
}
bool Server::handle_client(int client_socket) {
    std::string body = read_request(client_socket);
    if (body.empty())
        return false;
    Response response(client_socket, *this);
    response.request = Request(body);
    std::string method = response.request.getMethod();
    std::string path = response.request.getPath();
    if(!check_method(method, root_location.allowed_methods)) {
        response.send_error_response(405, "text/html", error_pages[405]);
        close(client_socket);
        return true;
    }

    std::cout << YELLOW << "[" << current_time() << "] Request method: " << method << ", Path: " << path << RESET << std::endl;

    if (method == "GET")
        response.handle_get_request(body);
    else if (method == "POST") {
        if(!check_method(method, upload_location.allowed_methods)) {
            response.send_error_response(405, "text/html", error_pages[405]);
            close(client_socket);
            return true;
        }
        response.handle_post_request(body);
    }
    else if (method == "DELETE")
        response.handle_delete_request(body);
    else{
        std::cout << "Method not allowed" << std::endl;
        response.send_error_response(405, "text/html", error_pages[405]);
    }

    partial_requests.erase(client_socket);
    close(client_socket);
    return true;
}
void Server::start_server() {

        int poll_count = poll(poll_fds.data(), poll_fds.size(),0);
        if (poll_count < 0)
            throw std::runtime_error("Error polling for events");
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (std::find(server_sockets.begin(), server_sockets.end(), poll_fds[i].fd) != server_sockets.end())
                    new_connection(poll_fds[i].fd);
                else 
                    {
                        if(handle_client(poll_fds[i].fd))
                            {poll_fds.erase(poll_fds.begin() + i);
                            i--;
                            }
                    }
            }
        }
}