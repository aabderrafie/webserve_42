#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "parsing.hpp"
#include "utils.hpp"

struct Token {
    std::string value;
    std::string type;
};

class Tokenizer {
    public:
        Tokenizer();
        ~Tokenizer();
        void tokenize(const std::string& configContent);
        std::vector<Token> getTokens() const { return tokens; }
    private:
        std::vector<Token> tokens;
};