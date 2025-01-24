#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <string>

#define PORT 80
#define BACKLOG 10
#define BUFFER_SIZE 1024
#define STATIC_DIR "/var/www/html"


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

std::map<std::string, std::string> mime_types = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".gif", "image/gif"}
};

std::string get_mime_type(const std::string &path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string ext = path.substr(dot_pos);
        std::map<std::string, std::string>::iterator it = mime_types.find(ext);
        if (it != mime_types.end()) {
            return it->second;
        }
    }
    return "application/octet-stream";
}

void send_response(int client_socket, const std::string &status, const std::string &content_type, const std::string &body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n" // this is the status line
             << "Content-Type: " << content_type << "\r\n" // this is the content type aka MIME type
             << "Content-Length: " << body.size() << "\r\n" // this is the content length
             << "Connection: close\r\n" // we can't handle persistent connections
             << "\r\n" // blank line to separate headers from body
             << body; // the body of the response

    std::string response_str = response.str();

    send(client_socket, response_str.c_str(), response_str.size(), 0);
}


void handle_get_request(int client_socket, const std::string &uri) {
    std::string sanitized_uri = uri;
    std::cout << "URI: " << sanitized_uri << std::endl;
    if (sanitized_uri.find("..") != std::string::npos) {
        std::cout << "URI contains '..'!" << std::endl;
        std::string body = read_html_file("/var/www/html/403.html");
        send_response(client_socket, "403 Forbidden", "text/html", body);
        return;
    }

    std::string file_path = STATIC_DIR + sanitized_uri;
    std::cout << "File path: " << file_path << std::endl;
    if (file_path.back() == '/') {
        file_path += "ok.html";
    }


    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::string body = read_html_file("/var/www/html/404.html");
        send_response(client_socket, "404 Not Found", "text/html", body);
        return;
    }

    std::ostringstream file_content;
    file_content << file.rdbuf();
    std::string body = file_content.str();

    std::string content_type = get_mime_type(file_path);

    send_response(client_socket, "200 OK", content_type, body);
}


void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }
    buffer[bytes_received] = '\0';
    // std::cout << "---------------------------- Received ----------------------------" << std::endl;
    // std::cout << buffer << std::endl;
    // std::cout << "-------------------------------------------------------------------" << std::endl;
    std::istringstream request(buffer);
    std::string method, uri, version;
    request >> method >> uri >> version;
    handle_get_request(client_socket, uri);
    close(client_socket);
}


void start_server() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0, 8);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(server_socket, BACKLOG);

    std::cout << "Server is running on port " << PORT << std::endl;

    while (true) {
        sin_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sin_size);

        std::cout << "Connection from " << inet_ntoa(client_addr.sin_addr) << std::endl;
        handle_client(client_socket);

    }

    close(server_socket);
}


int main() {
    start_server();
    return 0;
}