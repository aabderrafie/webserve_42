#pragma once 
#include "../includes/include.hpp"

class Location {
public:
    std::string root;
    std::vector<std::string> allowed_methods;
    bool directory_listing;
    std::vector<std::string> cgi_extensions;
    std::string default_file;
    Location() : root("/files/html"), allowed_methods({"GET", "POST"}), 
                directory_listing(false), cgi_extensions({".py", ".php"}) {}
};
