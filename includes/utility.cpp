#include "include.hpp"

    std::string tostring(int value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }


    bool isDirectory(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        std::cout << "Directory found" << std::endl;
        closedir(dir);
        return true;
    }
    return false;
}
