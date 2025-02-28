#include "server.hpp"
#include "MultiServer.hpp"  
#include "Response.hpp"
#include "../includes/include.hpp"

int main(int ac, char **av) {
    try {
        if (ac != 2)
            throw std::runtime_error("Invalid number of arguments");
        std::string filename(av[1]);
        if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".conf")
            throw std::runtime_error("Invalid extension");
        Config config(av[1]);
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