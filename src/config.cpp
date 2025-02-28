#include "config.hpp"
#include "../config/parsing.hpp"

void Config::printConfig(){
    for (size_t i = 0; i < servers.size(); i++){
        std::cout << "\n\n$$$\n\nServer " << i << std::endl;
        for (size_t j = 0; j < servers[i].ports.size(); j++)
            std::cout << "Port: " << servers[i].ports[j] << std::endl;
        std::cout << "Error pages: " << std::endl;
        for (std::map<int, std::string>::iterator it = servers[i].error_pages.begin(); it != servers[i].error_pages.end(); ++it)
            std::cout << it->first << ": " << it->second << std::endl;
        std::cout << "Locations: " << std::endl;
        for (std::map<std::string, Location>::iterator it = servers[i].locations.begin(); it != servers[i].locations.end(); ++it){
            std::cout << "\n" << it->first << ": " << std::endl;
            std::cout << "Root: " << it->second.root << std::endl;
            std::cout << "Index: " << it->second.default_file << std::endl;
            std::cout << "Allowed methods: ";
            for (std::vector<std::string>::iterator it2 = it->second.allowed_methods.begin(); it2 != it->second.allowed_methods.end(); ++it2)
                std::cout << *it2 << "\t";
            std::cout << std::endl;
            std::cout << "Directory listing: " << it->second.directory_listing << std::endl;
            std::cout << "Allow upload: " << it->second.allow_upload << std::endl;
            std::cout << "CGI-Extensions: \n";
            for (std::map<std::string, std::string>::iterator it2 = it->second.cgi.begin(); it2 != it->second.cgi.end(); ++it2)
                std::cout << it2->first << " -> " << it2->second << "\n";
            std::cout << std::endl;
        }
    }
}

Config::Config(std::string path){
    parser p;
    p.parse(path);
    servers = initConfig(p.getConfig());
}
