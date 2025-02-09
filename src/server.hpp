#pragma once
#include "../includes/include.hpp"
#include "config.hpp" 

class Response;  // Forward declaration
class Request;   // Forward declaration
#include "location.hpp"

class Server {
    private:
    bool is_data_received = false;
    public:
        std::vector<int> server_sockets;
        std::vector<struct sockaddr_in> server_addrs;
        std::vector<struct pollfd> poll_fds;
        std::string host;
        std::vector<int> ports;
        std::string server_name;
        std::map<int, std::string> error_pages;
        int client_max_body_size;
        std::map<std::string, Location> locations;
        // Location root_location;   //replaced by locations[/]
        // Location upload_location; //replaced by locations[/upload]
        // Location cgi_location;    //replaced by locations[/cgi-bin]

        Server();
        ~Server();
        void server_init();
        void start_server();
        void new_connection(int server_socket);
        void bind_and_listen();
        bool handle_client(int client_socket);
        std::string read_request(int client_socket);
        size_t get_content_length(const std::string& request);
        bool check_method(const std::string& method, const std::vector<std::string>& allowed_methods);
        

};


std::string current_time();
#include "Response.hpp"
#include "Request.hpp"