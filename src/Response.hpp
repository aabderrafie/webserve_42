#pragma once

#include "mime_types.hpp"
#include "Request.hpp"
#include "../includes/include.hpp"
#include "server.hpp"
#include <filesystem> // Add this line

class Server;

class Response {
    private:
        int client_socket;
        MimeTypes mime_types;
        int status;
        std::string content_type;
        std::string body;
        Server server;

    public:
        Response(int client_socket, const Server& server);
        Request request;
        void send_response();
        std::string read_html_file(const std::string& file_path);
        void handle_get_request(const std::string &path);
        void set_content_type(const std::string &content_type);
        void set_status(int status);
        void set_body(const std::string &body);
        bool is_valid_url(const std::string& url);
        bool is_directory(const std::string& url);
        void send_error_response(int status, const std::string& content_type, const std::string& error_page_path);
        void handle_post_request(const std::string& path);
        void handle_delete_request(const std::string& path);
        void create_user(const std::map<std::string, std::string>& data, const std::string& uploads);
        void list_directory_contents(const std::string& dir_path);
        void check_error(const std::string& path);

        ~Response();
};