
#include "config.hpp"
#include "../config/parsing.hpp"

    Config::Config(){
        // Example server configuration
        Server server1;

        server1.host = "127.0.0.1";
        server1.ports.push_back(1337);
        server1.ports.push_back(8080);
        server1.ports.push_back(8081);
        server1.server_name = "webserver1234.com";
        server1.error_pages[301] = "./files/error_pages/301.html";
        server1.error_pages[400] = "./files/error_pages/400.html";
        server1.error_pages[403] = "./files/error_pages/403.html";
        server1.error_pages[404] = "./files/error_pages/404.html";
        server1.error_pages[405] = "./files/error_pages/405.html";
        server1.error_pages[409] = "./files/error_pages/409.html";
        server1.error_pages[413] = "./files/error_pages/413.html";
        server1.error_pages[414] = "./files/error_pages/414.html";
        server1.error_pages[500] = "./files/error_pages/500.html";
        server1.error_pages[501] = "./files/error_pages/501.html";
        server1.root_location.root = "./files/html";
        server1.upload_location.root = "./files/uploads";
        server1.root_location.default_file = "index.html";
        server1.upload_location.allowed_methods.push_back("POST");
        

        servers.push_back(server1);

        Server server2;
        server2.host = "127.0.0.1";
        server2.ports.push_back(1234);
        // server2.ports.push_back(42);

        server2.server_name = "webserver8080.com";
        server2.error_pages[301] = "./files/error_pages/301.html";
        server2.error_pages[400] = "./files/error_pages/400.html";
        server2.error_pages[403] = "./files/error_pages/403.html";
        server2.error_pages[404] = "./files/error_pages/404.html";
        server2.error_pages[405] = "./files/error_pages/405.html";
        server2.error_pages[409] = "./files/error_pages/409.html";
        server2.error_pages[413] = "./files/error_pages/413.html";
        server2.error_pages[414] = "./files/error_pages/414.html";
        server2.error_pages[500] = "./files/error_pages/500.html";
        server2.error_pages[501] = "./files/error_pages/501.html";
         server2.root_location.root = "/var/www/html";
        server2.upload_location.root = "/var/www/html/uploads";
        server2.root_location.default_file = "success.html";

        servers.push_back(server2);
        // parser p;
        // p.parse("/home/abderrafie/Desktop/Webserv/config/full.conf");
        // initConfig(p.getConfig());
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
        std::cout << "Error Pages: " << std::endl;
        for (const auto& error_page : server.error_pages) {
            std::cout << "  " << error_page.first << ": " << error_page.second << std::endl;
        }
        std::cout << std::endl;
    }
}