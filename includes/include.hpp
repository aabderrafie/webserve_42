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