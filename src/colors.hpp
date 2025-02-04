// colors.hpp
#pragma once
#include <string>
#include <iostream>
#include "../includes/include.hpp"
namespace Draw {
    inline void drawBox(const std::string& title) {
        std::cout << CYAN << "\n╔═════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    " << title << "                         ║\n";
        std::cout << "╚═════════════════════════════════════════════════════════════════╝\n" << RESET << std::endl;
    }

    inline void serverStart(int serverNum, const std::string& ports) {
        std::cout << BLUE << "⚡ Initializing Server [" << serverNum << "]..." << RESET << std::endl;
        std::cout << GREEN << "✓ Server [" << serverNum << "] bound to ports: {" 
                 << YELLOW << ports << GREEN << "}" << RESET << std::endl;
        std::cout << PURPLE << "✨ Server [" << serverNum << "] ready to accept connections" 
                 << RESET << std::endl << std::endl;
    }

    inline void serverClose(int serverNum) {
        std::cout << YELLOW << "🔸 Closing server socket [" << serverNum << "]" << RESET << std::endl;
    }

    inline void success(const std::string& message) {
        std::cout << GREEN << "✓ " << message << RESET << std::endl;
    }

    inline void error(const std::string& message) {
        std::cerr << RED << "❌ Error: " << message << RESET << std::endl;
    }

    inline void info(const std::string& message) {
        std::cout << BLUE << "ℹ️  " << message << RESET << std::endl;
    }

    inline void warning(const std::string& message) {
        std::cout << YELLOW << "⚠️  " << message << RESET << std::endl;
    }
}