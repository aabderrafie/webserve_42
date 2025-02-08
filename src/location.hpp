#pragma once 
#include "../includes/include.hpp"

class Location {
public:
    std::string root;
    bool directory_listing;
    std::string default_file;
    std::vector<std::string> allowed_methods;
    std::vector<std::string> cgi_extensions;
    Location() {
        root = "./files/html";
        default_file = "index.html";
        directory_listing = false;
        // allowed_methods.push_back("GET");
        // allowed_methods.push_back("POST");
        // cgi_extensions.push_back(".py");
        // cgi_extensions.push_back(".php");
    }
};
