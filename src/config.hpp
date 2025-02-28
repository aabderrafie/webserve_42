#pragma once
#include <string>
#include <map>
#include <vector>
#include "../includes/include.hpp"
#include "server.hpp"
class Server;

class Config {
    public:
    void printConfig();
    Config(std::string path);
    std::vector<Server> servers;
};