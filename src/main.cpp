#include "server.hpp"
#include "MultiServer.hpp"  
#include "Response.hpp"
#include "../includes/include.hpp"

int main() {
    try {
        Config config;
        MultiServer server(config);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}