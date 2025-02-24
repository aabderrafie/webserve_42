#pragma once
#include "../includes/include.hpp"
#include "config.hpp" 

class Response;  // Forward declaration
class Request;   // Forward declaration
#include "location.hpp"
#include"../config/utils.hpp"


class Session {
    public:
        int session_id;
        std::map<std::string, std::string> session_data;

        Session() {
            std::cout << "Session created" << std::endl;
        }

        Session(int id, const std::string& data): session_id(id) {
            std::vector<std::string> vars = split(data, ';');
            for (std::vector<std::string>::iterator it = vars.begin(); it != vars.end(); ++it) {
                std::vector<std::string> key_value = split(*it, '=');
                if (key_value.size() == 2) {
                    session_data[key_value[0]] = key_value[1];
                }
            }
            std::cout << "Session created" << std::endl;
        }

        ~Session() {}

    private:
        std::vector<std::string> split(const std::string& s, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(token);
            }
            return tokens;
        }
};

class Server {
    public:
        // std::map<int, std::string> partial_requests;
        bool is_cgi(std::string path, std::string &extension);// zouhir add this
        void send_cgi(std::string extension, std::string path, int client_socket, Response& response);//zouhir add this
        std::vector<int> server_sockets;
        std::vector<struct sockaddr_in> server_addrs;
        std::vector<struct pollfd> poll_fds;
        std::string host;
        std::vector<int> ports;
        std::string server_name;
        std::map<int, std::string> error_pages;
        int client_max_body_size;
        std::map<std::string, Location> locations;
        std::map<int, Session> sessions;


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
        
        // void save_sessions_to_file(); // Add this line
        // void load_sessions_from_file(); // Add this line
};

// int getSessionID(const std::string& ref);

std::string current_time();
#include "Response.hpp"
#include "Request.hpp"