#include "server.hpp"
#include "MultiServer.hpp"  
#include "Response.hpp"
#include "../includes/include.hpp"

int main() {
    try {
        Config config;
        std::cout << BLUE << "⚡ Loading configuration..." << RESET << std::endl;
        MultiServer server(config);
        std::cout << GREEN << "✓ Configuration loaded successfully!" << RESET << std::endl;
        std::cout << YELLOW << "🚀 Starting server..." << RESET << std::endl;
        server.start_servers();
        std::cout << PURPLE << "✨ Server is running!" << RESET << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << RED << "❌ Error: " << e.what() << RESET << std::endl;
    }
    return 0;
}