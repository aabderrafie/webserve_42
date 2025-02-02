#pragma once

#include "tokenizer.hpp"
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
	std::map<std::string, std::string> directives;
	std::vector<block> recursive_blocks;
};

class WebServerConfig;

class parser {
	public:
		parser();
		~parser();
		WebServerConfig parse( const std::string& configContent );
		block parseBlock( std::vector<Token>::iterator& current, std::vector<Token>::iterator end );
		void printConfig();
		std::vector<block> getConfig() const { return (blocks); }
	private:
		std::vector<block> blocks;
};

class WebServerConfig {
	public:
		struct Server {
			std::string					host; 					// Host address (e.g., "localhost")
			int							port;					// Port number (e.g., 8080)
			std::vector<std::string>	server_names; 			// List of server names
			bool						is_default_server;		// Is this the default server for the host:port?
			std::map<int, std::string>	error_pages;			// Map of error codes to error page paths
			std::string					client_max_body_size;	// Maximum client body size (e.g., "1M")
			struct Route {
				std::string path; 							// URL path
				std::vector<std::string> allowed_methods;	// List of allowed HTTP methods (e.g., GET, POST)
				std::string redirect;						// HTTP redirection URL
				std::string root_directory;					// Directory or file root for the route
				bool directory_listing;						// Enable or disable directory listing
				std::string default_file;					// Default file to serve if the request is a directory
				std::string cgi_extension;					// File extension for CGI execution (e.g., .php)
				std::string upload_path;					// Directory to save uploaded files
				Route( block& ref );			// Default constructor
			};
			std::vector<Route>			routes;						// List of routes for this server
			Server(block& ref);
		};
		std::vector<Server> servers;
		size_t size; 
		WebServerConfig( std::vector<block> blocks );

		Server getServer( int index ) {
			return (servers[index]);
		}

};
