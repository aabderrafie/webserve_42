#pragma once 

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <map> // Replace unordered_map with map
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
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/types.h>

#define PORT 
#define BACKLOG 2
#define BUFFER_SIZE 1024
#define clinet_max 1000000000

#include <ctime>
#define RESET         "\033[0m"
#define RED          "\033[1;31m"      // Bold bright red
#define GREEN        "\033[1;32m"      // Bold bright green
#define YELLOW       "\033[1;33m"      // Bold bright yellow
#define BLUE         "\033[1;34m"      // Bold bright blue
#define MAGENTA      "\033[1;35m"      // Bold bright magenta
#define CYAN         "\033[1;36m"      // Bold bright cyan
#define WHITE        "\033[1;37m"      // Bold bright white
#define ORANGE       "\033[1;38;5;214m" // Bold vibrant orange
#define PURPLE       "\033[1;38;5;135m" // Bold rich purple
#define PINK         "\033[1;38;5;219m" // Bold hot pink
#define LIME         "\033[1;38;5;154m" // Bold bright lime
#define TEAL         "\033[1;38;5;44m"  // Bold bright teal
#define GOLD         "\033[1;38;5;226m" // Bold bright gold
#define CRIMSON      "\033[1;38;5;196m" // Bold deep red
#define VIOLET       "\033[1;38;5;171m" // Bold bright violet
#define TURQUOISE    "\033[1;38;5;45m"  // Bold bright turquoise
#define CORAL        "\033[1;38;5;209m" // Bold coral
#define LAVENDER     "\033[1;38;5;147m" // Bold lavender
#define EMERALD      "\033[1;38;5;46m"  // Bold emerald green