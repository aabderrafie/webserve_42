# pragma once
#include <string>
#include <map>
#include <sstream>
#include <unistd.h>
#include "config.hpp"
#include "mime_types.hpp"
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <poll.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <cctype>
#include "Request.hpp"

class Response {
    private:
        int client_socket;
        Config config;
        MimeTypes mime_types;
        int status;
        std::string content_type;
        std::string body;

    public:
        Response(int client_socket, const Config& config);
        void send_response();
        std::string read_html_file(const std::string& file_path);
        void handle_get_request(const std::string &uri, const Config& config);
        void set_content_type(const std::string &content_type);
        void set_status(int status);
        void set_body(const std::string &body);
        bool is_valid_url(const std::string& url);
        bool is_directory(const std::string& url);
        void send_error_response(int status, const std::string& content_type, const std::string& error_page_path);
        void handle_post_request(const std::string& uri, const std::string& body, const Config& config);
        void create_user(const std::map<std::string, std::string>& data, const std::string& uploads);
        void handle_binary_file(const std::string& uri);

        ~Response();

};

