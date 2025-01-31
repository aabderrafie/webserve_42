#pragma once 

#include <iostream>
#include <string>
#include <vector>

class Config {
public:
        std::vector<int> listen_ports;

    std::string server_name;
    std::vector<std::pair<int, std::string>> error_pages;

    struct Location {
        std::string path;
        std::string root;
        std::vector<std::string> allow_methods;
        bool autoindex;
    };

    Location root_location;
    Location uploads_location;

    Config() {
        // Default initialization
        listen_ports = {8080,80,4242,1337};
        server_name = "localhost";
// 301.html  400.html  403.html  404.html  405.html  409.html  413.html  414.html  500.html  501.html
        error_pages = {
            {404, "/error_pages/404.html"},
            {405, "/error_pages/405.html"},
            {403, "/error_pages/403.html"},
            {500, "/error_pages/500.html"},
            {501, "/error_pages/501.html"},
            {400, "/error_pages/400.html"},
            {409, "/error_pages/409.html"},
            {413, "/error_pages/413.html"},
            {414, "/error_pages/414.html"}
        };

        root_location = {
            "/", "/var/www/html", {"GET", "POST", "DELETE"}, false
        };

        uploads_location = {
            "/uploads", "/var/www/html/uploads", {"POST"}, false
        };
    }

    // void print_config() const {
    //     std::cout << "Server Configuration:" << std::endl;
    //     std::cout << "Listen on port: " << listen_port << std::endl;
    //     std::cout << "Server name: " << server_name << std::endl;

    //     std::cout << "Error pages:" << std::endl;
    //     for (const auto& error_page : error_pages) {
    //         std::cout << "  Error " << error_page.first << ": " << error_page.second << std::endl;
    //     }

    //     std::cout << "Root location:" << std::endl;
    //     std::cout << "  Path: " << root_location.path << std::endl;
    //     std::cout << "  Root: " << root_location.root << std::endl;
    //     std::cout << "  Allowed methods: ";
    //     for (const auto& method : root_location.allow_methods) {
    //         std::cout << method << " ";
    //     }
    //     std::cout << "\n  Autoindex: " << (root_location.autoindex ? "on" : "off") << std::endl;

    //     std::cout << "Uploads location:" << std::endl;
    //     std::cout << "  Path: " << uploads_location.path << std::endl;
    //     std::cout << "  Root: " << uploads_location.root << std::endl;
    // }
};

