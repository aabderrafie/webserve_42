
#include "config.hpp"
#include "../config/parsing.hpp"

Config::Config(){
    parser p;
    p.parse("/home/taha/Documents/Webserv/config/full.conf");
    servers = initConfig(p.getConfig());
    printServerDetails();
}

static void printLocationDetails(const Location& location) {
    std::cout << "    Root: " << location.root << std::endl;
    std::cout << "    Default File: " << location.default_file << std::endl;
    std::cout << "    Directory Listing: " << location.directory_listing << std::endl;
    std::cout << "    Allowed Methods: ";
    for (const auto& method : location.allowed_methods) {
        std::cout << method << " ";
    }
    std::cout << std::endl;
    std::cout << "    CGI Extensions: ";
    for (const auto& cgi_ext : location.cgi_extensions) {
        std::cout << cgi_ext << " ";
    }
    std::cout << std::endl;
}

void Config::printServerDetails() const {
    for (const auto& server : servers) {
        std::cout << "Host: " << server.host << std::endl;
        std::cout << "Ports: ";
        for (const auto& port : server.ports) {
            std::cout << port << " ";
        }
        std::cout << std::endl;
        std::cout << "Server Name: " << server.server_name << std::endl;
        std::cout << "Client Max Body Size: " << server.client_max_body_size << std::endl;
        std::cout << "Error Pages: " << std::endl;
        if (server.error_pages.empty()) {
            std::cout << "  None" << std::endl;
        } else {
            for (const auto& error_page : server.error_pages) {
                std::cout << "  " << error_page.first << ": " << error_page.second << std::endl;
            }
        }
        std::cout << "Locations: " << std::endl;
        std::cout << "  Root Location: " << std::endl;
        printLocationDetails(server.root_location);
        std::cout << "  Upload Location: " << std::endl;
        printLocationDetails(server.upload_location);
        std::cout << "  CGI Location: " << std::endl;
        printLocationDetails(server.cgi_location);
        std::cout << std::endl;
    }
}