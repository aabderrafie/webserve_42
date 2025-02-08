
#include "config.hpp"
#include "../config/parsing.hpp"

Config::Config(){
    parser p;
    p.parse("/home/abderrafie/Desktop/Webserv/config/full.conf");
    servers = initConfig(p.getConfig());
}
