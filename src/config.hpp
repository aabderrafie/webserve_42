#pragma once
#include <string>
#include <map>
#include <vector>
#include "../includes/include.hpp"
#include "server.hpp"
class Server;

class Config {
public:
    std::vector<Server> servers;
    Config();
    void printServerDetails() const;
};