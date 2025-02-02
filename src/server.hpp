#pragma once
#include "../includes/include.hpp"
#include "config.hpp" 

class Response;  // Forward declaration
class Request;   // Forward declaration
#include "location.hpp"

class Server {
    private:

    public:
        std::vector<int> server_sockets;
        std::vector<struct sockaddr_in> server_addrs;
        std::vector<struct pollfd> poll_fds;
        std::string host;
        std::vector<int> ports;
        std::string server_name;
        std::map<int, std::string> error_pages;
        int client_max_body_size;
        Location root_location;
        Location upload_location;
        Location cgi_location;
        Server();
        ~Server();
        bool is_client_socket(int fd);
        bool is_server_socket(int fd) ;
        // Server(const Server& server);
        void server_init();
        void start_server();
        void new_connection(int server_socket);
        void bind_and_listen();
        void handle_client(int client_socket);
        std::string read_request(int client_socket);
};
std::string current_time();
#include "Response.hpp"
#include "Request.hpp"