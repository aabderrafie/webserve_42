#pragma once 

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <string>
#include <sys/stat.h>
#include <poll.h>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <sys/socket.h>
#include <ctime>
#include <iomanip>
#include <cctype>

#define PORT 80
#define BACKLOG 10
#define BUFFER_SIZE 1024
#define clinet_max 1000000000

#include "../src/Response.hpp"
#include "../src/Request.hpp"
#include "../src/MultiServer.hpp"
#include "../src/server.hpp"
#include "../src/config.hpp"

#include <ctime>
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"