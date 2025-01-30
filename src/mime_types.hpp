# pragma once
#include <string>
#include <map>
#include <iostream>

class MimeTypes {
    private:
        std::map<std::string, std::string> mime_types;
    public:
        MimeTypes();
        ~MimeTypes();
        std::string get_mime_type(const std::string &path);
};