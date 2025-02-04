#pragma once 
#include "../includes/include.hpp"

class Location {
public:
    std::string root;
    bool directory_listing;
    std::string default_file;
    std::vector<std::string> allowed_methods;
    std::vector<std::string> cgi_extensions;
    Location() : root("/files/html"), directory_listing(false) {}
    // Location() : root("/files/html"), allowed_methods({"GET", "POST"}), 
    //             directory_listing(false), cgi_extensions({".py", ".php"}) {}
};
