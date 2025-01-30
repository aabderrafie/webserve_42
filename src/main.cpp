#include "server.hpp"
#include "config.hpp"
#include "Response.hpp"

int main() {
    try {
        Config config;
        Server server(config);
        server.start_server();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}