
#include "config.hpp"
#include "../config/parsing.hpp"

Config::Config(){
    parser p;
    p.parse("./config/full.conf");
    servers = initConfig(p.getConfig());
}
