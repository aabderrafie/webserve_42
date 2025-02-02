#include "server.hpp"
#include "MultiServer.hpp"  
#include "Response.hpp"
#include "../includes/include.hpp"

int main() {
    try {
        Config config;
        std::cout << "Configuring servers..." << std::endl;
        
        MultiServer server(config);
        server.start_servers();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}