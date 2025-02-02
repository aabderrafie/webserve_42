#pragma once
#include"../includes/include.hpp"

class Server {
    private:
        std::vector<int> server_sockets;
        std::vector<struct sockaddr_in> server_addrs;
        std::vector<struct pollfd> poll_fds;

    public:
        std::string host; // default is 127.0.0.1
        std::vector<int> ports; // default is 80, 1337
        std::string server_name; // default is localhost
        std::map<int, std::string> error_pages;
        int client_max_body_size; // default is 1MB
        Location root_location; // default is /files/html
        Location upload_location; // default is /files/upload
        Location cgi_location; // default is /files/cgi-bin
        Server();
        ~Server();
        void server_init();
        void start_server();
        void new_connection(int server_socket);
        void bind_and_listen();
        void handle_client(int client_socket);
        std::string read_request(int client_socket);
};

std::string current_time();