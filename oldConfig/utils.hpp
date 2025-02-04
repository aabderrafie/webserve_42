#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::string readFile(const std::string &filePath);
std::vector<std::string> split(const std::string &str, char delimiter);