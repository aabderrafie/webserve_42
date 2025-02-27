#include "../includes/include.hpp"

std::string tostring(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


bool isDirectory(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {

        closedir(dir);
        return true;
    }
    return false;
}

std::string current_time() {
    std::time_t now = std::time(NULL);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

void Message(const std::string &message, std::string color) {
    std::cout << message << std::endl;
    std::cout << color << "[" << current_time() << "] " << message << RESET << std::endl;
}