#pragma once 
#include "../includes/include.hpp"

class Location {
public:
    std::string root;
    bool directory_listing;
    bool allow_upload;
    bool have_redirect;
    std::string redirect;
    std::string default_file;
    std::vector<std::string> allowed_methods;
    std::map<std::string, std::string> cgi;
    Location() {}
};
