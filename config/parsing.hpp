#pragma once

#include "tokenizer.hpp"
#include "../src/location.hpp"
#include "../src/config.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Token;

struct block {
	std::string block_name;
	std::map<std::string, std::vector<std::string> > directives;
	std::vector<block> recursive_blocks;
};

class Config;

class parser {
	public:
		parser();
		~parser();
		void parse( const std::string& configContent );
		block parseBlock( std::vector<Token>::iterator& current, std::vector<Token>::iterator end );
		std::vector<block> getConfig() const { return (blocks); }
	private:
		std::vector<block> blocks;
};

void configureLocation( block& ref, Location& loc );
Server configureServer( block& ref );
std::vector<Server> initConfig( std::vector<block> blocks );