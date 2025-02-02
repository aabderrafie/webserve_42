#include <string>
#include <map>
#include <vector>
#include "../includes/include.hpp"
#include "server.hpp"
class Location {
public:
    std::string root; //default is /files/html
    std::vector<std::string> allowed_methods; //default is GET POST
    bool directory_listing; // default is false
    std::vector<std::string> cgi_extensions; //default is .py .php
    std::string default_file; //default is index.html
    Location() : root("/files/html"), allowed_methods({"GET", "POST"}), directory_listing(false), cgi_extensions({".py", ".php"}) {}
};


class Config {
public:
    std::vector<Server> servers;
    size_t _size;

    Config() {
        // Example server configuration
        Server server1;
        server1.host = "127.0.0.1";
        server1.ports.push_back(8080);
        server1.ports.push_back(4242);
        server1.ports.push_back(1337);
        server1.ports.push_back(80);
        server1.server_name = "localhost webserver.com";
        server1.error_pages[301] = "/files/error_pages/301.html";
        server1.error_pages[400] = "/files/error_pages/400.html";
        server1.error_pages[403] = "/files/error_pages/403.html";
        server1.error_pages[404] = "/files/error_pages/404.html";
        server1.error_pages[405] = "/files/error_pages/405.html";
        server1.error_pages[409] = "/files/error_pages/409.html";
        server1.error_pages[413] = "/files/error_pages/413.html";
        server1.error_pages[414] = "/files/error_pages/414.html";
        server1.error_pages[500] = "/files/error_pages/500.html";
        server1.error_pages[501] = "/files/error_pages/501.html";

        servers.push_back(server1);

        // Example second server configuration
        Server server2;
        server2.host = "127.0.0.1";
        server2.ports.push_back(1234);
        server2.ports.push_back(4321);
        server2.server_name = "localhost";
        server2.error_pages[301] = "/files/error_pages/301.html";
        server2.error_pages[400] = "/files/error_pages/400.html";
        server2.error_pages[403] = "/files/error_pages/403.html";
        server2.error_pages[404] = "/files/error_pages/404.html";
        server2.error_pages[405] = "/files/error_pages/405.html";
        server2.error_pages[409] = "/files/error_pages/409.html";
        server2.error_pages[413] = "/files/error_pages/413.html";
        server2.error_pages[414] = "/files/error_pages/414.html";
        server2.error_pages[500] = "/files/error_pages/500.html";
        server2.error_pages[501] = "/files/error_pages/501.html";

        servers.push_back(server2);
    }
};