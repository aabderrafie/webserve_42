
#include "server.hpp"
#include "config.hpp"
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include "Response.hpp"

std::map<std::string, std::string> mime_types = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".gif", "image/gif"}
};

std::string read_html_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << file_path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string get_mime_type(const std::string &path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string ext = path.substr(dot_pos);
        auto it = mime_types.find(ext);
        if (it != mime_types.end()) {
            return it->second;
        }
    }
    return "application/octet-stream";
}

void send_response(int client_socket, const std::string &status, const std::string &content_type, const std::string &body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n"
             << "Content-Type: " << content_type << "\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << body;

    std::string response_str = response.str();
    send(client_socket, response_str.c_str(), response_str.size(), 0);
}


void handle_get_request(int client_socket, const std::string &uri, const Config& config) {
    std::string sanitized_uri = uri;
    if (sanitized_uri.find("..") != std::string::npos) {
        std::cout << "URI contains '..'!" << std::endl;
        std::string body = read_html_file(config.root_location.root + config.error_pages[2].second);
        send_response(client_socket, "403 Forbidden", "text/html", body);
        return;
    }

    std::string file_path = config.root_location.root + sanitized_uri;
    if (file_path.back() == '/')
        file_path += "ok.html";
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::string body = read_html_file(config.root_location.root + config.error_pages[0].second);
        send_response(client_socket, "404 Not Found", "text/html", body);
        return;
    }

    std::ostringstream file_content;
    file_content << file.rdbuf();
    std::string body = file_content.str();
    std::string content_type = get_mime_type(file_path);
    send_response(client_socket, "200 OK", content_type, body);
}

void handle_post_request(int client_socket, const std::string &uri, const std::string &request_body, const Config& config) {
    if (uri.find("..") != std::string::npos) {
        send_response(client_socket, "403 Forbidden", "text/plain", "Forbidden");
        return;
    }

    std::string upload_dir = config.uploads_location.root + "/";
    std::string filename = upload_dir + std::to_string(time(nullptr)) + "_upload";

    mkdir(upload_dir.c_str(), 0755);

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        send_response(client_socket, "500 Internal Server Error", "text/plain", "Could not create file");
        return;
    }
    outfile.write(request_body.c_str(), request_body.size());
    outfile.close();

    std::string response_body = "File uploaded successfully: " + filename;
    send_response(client_socket, "201 Created", "text/plain", response_body);
}

void handle_client(int client_socket, const Config& config) {
    Response response(client_socket, config);

    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }

    buffer[bytes_received] = '\0';
    std::istringstream request(buffer);
    std::string method, uri, version;
    request >> method >> uri >> version;
    std::cout << "Method: " << method << std::endl;
    if (method == "GET")
        response.handle_get_request(uri, config);

    else {
        std::string body = read_html_file(config.root_location.root + config.error_pages[1].second);
        send_response(client_socket, "405 Method Not Allowed", "text/html", body);
    }
    close(client_socket);
}

void start_server(const Config& config) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config.listen_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0, 8);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, BACKLOG);

    std::cout << "Server is running on port " << config.listen_port << std::endl;

    std::vector<struct pollfd> poll_fds;
    struct pollfd server_poll_fd;
    server_poll_fd.fd = server_socket;
    server_poll_fd.events = POLLIN;

    poll_fds.push_back(server_poll_fd);

    while (true) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);

        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == server_socket) {
                    sin_size = sizeof(client_addr);
                    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sin_size);
                    struct pollfd client_poll_fd;
                    client_poll_fd.fd = client_socket;
                    client_poll_fd.events = POLLIN;
                    poll_fds.push_back(client_poll_fd);
                } else {
                    handle_client(poll_fds[i].fd, config);
                    poll_fds.erase(poll_fds.begin() + i);
                    --i;
                }
            }
        }
    }

    close(server_socket);
}

int main(int argc, char** argv) {
    Config config;
    start_server(config);
    return 0;
}