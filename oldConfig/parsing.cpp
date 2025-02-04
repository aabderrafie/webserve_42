#include "parsing.hpp"

parser::parser() {}

parser::~parser() {}

static void handleDirective(block& ret, std::vector<Token>::iterator& current) {
	std::string tmp = current->value;
	if ((current + 2)->value == ";")
		ret.directives[tmp] += (current + 1)->value;
	else {
		while ((current + 1)->type != "symbol") {
			ret.directives[tmp] += (current + 1)->value;
			ret.directives[tmp] += ":";
			current++;
		}
		if ((current + 1)->type != "symbol")
			throw std::runtime_error("Error: expected semicolon " + current->value);
	}
}

block parser::parseBlock(std::vector<Token>::iterator& current, std::vector<Token>::iterator end) {
	block	ret;
	int		i;

	i = 0;
	if (current->value == "server")
		ret.block_name = current->value;
	else
		ret.block_name = (++current)->value;
	++current;
	if (current->value != "{")
		throw std::runtime_error("Error: expected opening bracket");
	while (current < end) {
		if (current->value == "{")
			i++;
		if (current->value == "}")
			i--;
		if (current->value == "}" && i == 0)
			break;
		if (current->type == "directive") {
			handleDirective(ret, current);
		} else if (current->type == "block") {
			if (current->value == "location" && ret.block_name == "server") {
				ret.recursive_blocks.push_back(parseBlock(current, end));
			} else {
				throw std::runtime_error("Error: unautorized token: " + current->value);
			}
		}
		current++;
	} if (i != 0) {
		if (i > 0) throw std::runtime_error("Error: missing closing bracket");
		else throw std::runtime_error("Error: missing opening bracket");
	} return (ret);
}

WebServerConfig parser::parse(const std::string &configContent) {
	Tokenizer	tok;

	tok.tokenize(configContent);
	std::vector<Token> tokens = tok.getTokens();
	std::vector<Token>::iterator it = tokens.begin();
	while (it < tokens.end()) {
		if (it->type == "block" && it->value == "server")
			blocks.push_back(parseBlock(it, tokens.end()));
		else throw std::runtime_error("Error: unautorized token: " + it->value);
		it++;
	}
	std::vector<block> ret = getConfig(); 
	return WebServerConfig(ret);
}

void parser::printConfig() {
	int block_id = 0;
	for (std::vector<block>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
		std::cout << "block id " << block_id++ << ":" << std::endl;
		for (std::map<std::string, std::string>::iterator it2 = it->directives.begin(); it2 != it->directives.end(); ++it2) {
			std::cout << "   -" << it2->first << " = " << it2->second << std::endl;
		} for (std::vector<block>::iterator it2 = it->recursive_blocks.begin(); it2 != it->recursive_blocks.end(); ++it2) {
			std::cout << "   -recursive block name: " << it2->block_name << std::endl;
			for (std::map<std::string, std::string>::iterator it3 = it2->directives.begin(); it3 != it2->directives.end(); ++it3) {
				std::cout << "\t" << it3->first << " = " << it3->second << std::endl;
			}
		}
	}
}

static bool validateAllowedMethods(std::vector<std::string> ref) {
	for (std::vector<std::string>::iterator it = ref.begin(); it != ref.end(); ++it) {
		if (*it != "GET" && *it != "POST" && *it != "DELETE" && *it != "PUT" && *it != "HEAD")
			return false;
	}
	return true;
}	

static bool isValidPath(const std::string& path) {
    if (path.empty()) return false;

    const std::string invalidChars = "*?<>|\":";
    bool hasDot = false;

    for (size_t i = 0; i < path.size(); i++) {
        char c = path[i];
        if (invalidChars.find(c) != std::string::npos)
			return false;
        if (i > 0 && path[i] == '/' && path[i - 1] == '/')
			return false;
        if (c == '.' && i > 0 && path[i - 1] == '.')
			return false;
        if (c == '.')
			hasDot = true;
    }
    if (path.back() == '/' && hasDot)
		return false;
    return true;
} 

static bool isValidExt(std::string ref) {
	std::vector<std::string> ext = split(ref, ':');
	for (std::vector<std::string>::iterator it = ext.begin(); it != ext.end(); ++it) {
		if (it->empty())
			return false;
		if (it->front() != '.')
			return false;
	}
	return true;
}

static bool isValidLink(std::string ref) {
	if (ref.empty())
		return false;
	return true;
}

WebServerConfig::Server::Route::Route( block& ref ): directory_listing(false) {
	this->path = ref.block_name;
	for (std::map<std::string, std::string>::iterator it2 = ref.directives.begin(); it2 != ref.directives.end(); ++it2) {
		if (it2->first == "allowed_methods") {
			std::vector<std::string> tmp = split(it2->second, ':');
			if (!validateAllowedMethods(tmp))
				throw std::runtime_error("Error: allowed_methods: invalid argument");
			this->allowed_methods = tmp;
		} else if (it2->first == "redirect") {
			if (!isValidLink(it2->second))
				throw std::runtime_error("Error: redirect: invalid argument");
			this->redirect = it2->second;
		} else if (it2->first == "root_directory") {
			if (!isValidPath(it2->second))
				throw std::runtime_error("Error: root_directory: invalid argument");
			this->root_directory = it2->second;
		} else if (it2->first == "directory_listing") {
			if (it2->second == "on") this->directory_listing = true;
			else if (it2->second == "off") this->directory_listing = false;
			else throw std::runtime_error("Error: directory_listing: invalid argument");
		} else if (it2->first == "default_file") {
			if (!isValidPath(it2->second))
				throw std::runtime_error("Error: default_file: invalid argument");
			this->default_file = it2->second;
		} else if (it2->first == "cgi_extensions") {
			if (!isValidExt(it2->second))
				throw std::runtime_error("Error: cgi_extensions: invalid argument");
			this->cgi_extension = it2->second;
		} else if (it2->first == "upload_path") {
			if (!isValidPath(it2->second))
				throw std::runtime_error("Error: upload_path: invalid argument");
			this->upload_path = it2->second;
		} else throw std::runtime_error("Error: unautorized directive: " + it2->first);
	}
}

// WebServerConfig::Server::Route::Route( block& ref ): directory_listing(false) {
// 	for (std::map<std::string, std::string>::iterator it2 = ref.directives.begin(); it2 != ref.directives.end(); ++it2) {
// 		if (it2->first == "location") {
// 			std::cout << "\t-location block name: " << it2->second << std::endl;//debug
// 			this->path = it2->second;
// 		} else if (it2->first == "allowed_methods") {
// 			std::cout << "\t-allowed_methods: " << it2->second << std::endl;//debug
// 			this->allowed_methods = split(it2->second, ':');
// 		} else if (it2->first == "redirect") {
// 			std::cout << "\t-redirect: " << it2->second << std::endl;//debug
// 			this->redirect = it2->second;
// 		} else if (it2->first == "root_directory") {
// 			std::cout << "\t-root_directory: " << it2->second << std::endl;//debug
// 			this->root_directory = it2->second;
// 		} else if (it2->first == "directory_listing") {
// 			if (it2->second == "on"){
// 				std::cout << "\t-directory_listing: " << it2->second << std::endl;//debug	
// 				this->directory_listing = true;
// 			} else if (it2->second == "off"){
// 				std::cout << "\t-directory_listing: " << it2->second << std::endl;//debug
// 				this->directory_listing = false;
// 			} else throw std::runtime_error("Error: directory_listing: invalid argument");
// 		} else if (it2->first == "default_file") {
// 			std::cout << "\t-default_file: " << it2->second << std::endl;//debug
// 			this->default_file = it2->second;
// 		} else if (it2->first == "cgi_extensions") {
// 			std::cout << "\t-cgi_extensions: " << it2->second << std::endl;//debug
// 			this->cgi_extension = it2->second;
// 		} else if (it2->first == "upload_path") {
// 			std::cout << "\t-upload_path: " << it2->second << std::endl;//debug
// 			this->upload_path = it2->second;
// 		} else throw std::runtime_error("Error: unautorized directive: " + it2->first);
// 	}
// }

static bool validateHost(std::string input) {
	std::vector<std::string> host = split(input, '.');
	if (host.size() != 4)
		return false;
	for (std::vector<std::string>::iterator it = host.begin(); it != host.end(); ++it) {
		try {
			if (std::stoi(*it) < 0 || std::stoi(*it) > 255)
				return false;
		} catch (std::exception& e) {
			return false;
		}
	}
	return true;
}

static bool validatePort(std::string input) {
	try {
		int digitCount = 0;
		int tmp = std::stoi(input);
		while (tmp != 0) {
			tmp /= 10;
			digitCount++;
		}
		if (std::stoi(input) < 0 || std::stoi(input) > 65535 || digitCount != input.size())
			return false;
	} catch (std::exception& e) {
		return false;
	}
	return true;
}

static bool isValidServerName(const std::string& name) {
    if (name.empty())
		return false;
    for (size_t i = 0; i < name.size(); i++) {
        char c = name[i];
        if (!(std::isalnum(c) || c == '.' || c == '-'))
			return false;
        if (i > 0 && ((c == '.' && name[i - 1] == '.') || (c == '-' && name[i - 1] == '-')))
			return false;
    }
    if (name.front() == '.' || name.front() == '-' || name.back() == '.' || name.back() == '-')
		return false;
    size_t lastDot = name.rfind('.');
    if (lastDot != std::string::npos && name.size() - lastDot - 1 < 2)
		return false;
    return true;
}

static bool validateServerNames(const std::vector<std::string>& serverNames) {
	for (std::vector<std::string>::const_iterator it = serverNames.begin(); it != serverNames.end(); ++it) {
		if (!isValidServerName(*it))
			return false;
	} return true;
}

static bool validateErrorPages(std::vector<std::string> ref) {
	if (ref.size() % 2 != 0){
		throw std::runtime_error("Error: error_page: missing argument");
		return false;
	}
	int i = 0;
	for (std::vector<std::string>::iterator it = ref.begin(); it != ref.end(); ++it) {
		try {
			if (i % 2 == 0 && (std::stoi(*it) < 100 || std::stoi(*it) > 599))
				return false;
			if (i % 2 == 1 && !isValidPath(*it))
				return false;
		} catch (std::exception& e) {
			return false;
		}
		i++;
	}
	return true;	
}

static bool validateClientMaxBodySize(std::string ref /*10M*/) {
	std::string size = ref.substr(0, ref.size() - 1);
	int digitCount = 0;
	int tmp = std::stoi(size);
	while (tmp != 0) {
		tmp /= 10;
		digitCount++;
	}
	std::string unit = ref.substr(digitCount);
	if (unit != "M" && unit != "K")
		return false;
	try {
		if (std::stoi(size) < 0){
			return false;
		}
	} catch (std::exception& e) {
		return false;
	}
	return true;
}

WebServerConfig::Server::Server( block& ref ) : host("localhost"), port(8080), is_default_server(false), client_max_body_size("1M") {	
	for (std::map<std::string, std::string>::iterator it2 = ref.directives.begin(); it2 != ref.directives.end(); ++it2) {
		if (it2->first == "server_name") {
			std::vector<std::string> server_names = split(it2->second, ':');
			if (!validateServerNames(server_names)){
				throw std::runtime_error("Error: server_name: invalid argument");
			}
			for (std::vector<std::string>::iterator it3 = server_names.begin(); it3 != server_names.end(); ++it3)
				this->server_names.push_back(*it3);
		} else if (it2->first == "listen") {
			std::vector<std::string> listen = split(it2->second, ':');
			if (listen.size() == 2) {
				if (!validateHost(listen[0]) || !validatePort(listen[1]))
					throw std::runtime_error("Error: listen: invalid argument");
				this->host = listen[0];
				this->port = std::stoi(listen[1]);
			} else if (listen.size() == 1) {
				if (!validatePort(listen[0]))
					throw std::runtime_error("Error: listen: invalid argument");
				this->port = std::stoi(listen[0]);
			} else throw std::runtime_error("Error: listen: invalid argument");
		} else if (it2->first == "host") {
			if (!validateHost(it2->second))
				throw std::runtime_error("Error: host: invalid argument");
			this->host = it2->second;
		} else if (it2->first == "port") {
			if (!validatePort(it2->second))
				throw std::runtime_error("Error: port: invalid argument");
			this->port = std::stoi(it2->second);
		} else if (it2->first == "is_default_server") {
			if (it2->second == "true")
				this->is_default_server = true;
			else if (it2->second == "false")
				this->is_default_server = false;
			else throw std::runtime_error("Error: default_server: invalid argument");
		} else if (it2->first == "error_page") { 
			std::vector<std::string> error_page = split(it2->second, ':');
			if (!validateErrorPages(error_page))
				throw std::runtime_error("Error: error_page: invalid argument");
			int i = 0;
			while (i < error_page.size()) {
				this->error_pages[std::stoi(error_page[i])] = error_page[(i+1)];
				i += 2;
			}
		} else if (it2->first == "client_max_body_size") {
			if (!validateClientMaxBodySize(it2->second))
				throw std::runtime_error("Error: client_max_body_size: invalid argument");
			this->client_max_body_size = it2->second;
		}
	}
	 for (std::vector<block>::iterator it2 = ref.recursive_blocks.begin(); it2 != ref.recursive_blocks.end(); ++it2)
		this->routes.push_back(Route(*it2));
}

// int server_id = 0;//debug
// WebServerConfig::Server::Server( block& ref ) : host("localhost"), port(8080), is_default_server(false), client_max_body_size("1M") {
// 	server_id++;//debug
// 	std::cout << "server id " << server_id << ":" << std::endl; //debug	
// 	for (std::map<std::string, std::string>::iterator it2 = ref.directives.begin(); it2 != ref.directives.end(); ++it2) {
// 		// std::cout << "   -" << it2->first << " = " << it2->second << std::endl;//debug
// 		if (it2->first == "server_name") {
// 			std::vector<std::string> server_names = split(it2->second, ':');
// 			for (std::vector<std::string>::iterator it3 = server_names.begin(); it3 != server_names.end(); ++it3) {
// 				std::cout << "   -adding " << *it3 << " to server_names" << std::endl; //debug
// 				this->server_names.push_back(*it3);
// 			}
// 			std::cout << std::endl; //debug
// 		} else if (it2->first == "listen") {
// 			std::vector<std::string> listen = split(it2->second, ':');
// 			if (listen.size() == 2) {
// 				std::cout << "   -setting host to " << listen[0] << " and port to " << listen[1] << std::endl; //debug
// 				this->host = listen[0];
// 				this->port = std::stoi(listen[1]);
// 			} else if (listen.size() == 1) {
// 				std::cout << "   -setting host to " << listen[0] << std::endl; //debug
// 				this->port = std::stoi(listen[0]);
// 			} else throw std::runtime_error("Error: listen: invalid argument");
// 		} else if (it2->first == "is_default_server") {
// 			if (it2->second == "true") {
// 				std::cout << "   -is_default_server = " << it2->second << std::endl; //debug
// 				this->is_default_server = true;
// 			}
// 			else if (it2->second == "false") {
// 				std::cout << "   -is_default_server = " << it2->second << std::endl; //debug
// 				this->is_default_server = false;
// 			}
// 			else throw std::runtime_error("Error: default_server: invalid argument");
// 		} else if (it2->first == "error_page") {
// 			std::vector<std::string> error_page = split(it2->second, ':');
// 			int i = 0;
// 			while (i < error_page.size()) {
// 				std::cout << "   -assigning error page " << error_page[i] << " to " << error_page[(i+1)] << std::endl; //debug
// 				this->error_pages[std::stoi(error_page[i])] = error_page[(i+1)];
// 				i += 2;
// 			}
// 		} else if (it2->first == "client_max_body_size") {
// 			std::cout << "   -client_max_body_size = " << it2->second << std::endl; //debug
// 			this->client_max_body_size = it2->second;
// 		} 
// 	}  for (std::vector<block>::iterator it2 = ref.recursive_blocks.begin(); it2 != ref.recursive_blocks.end(); ++it2) {
// 		std::cout << "   -recursive block name: " << it2->block_name << std::endl;//debug
// 		this->routes.push_back(Route(*it2));
// 	}
// 	std::cout << std::endl; //debug
// }

WebServerConfig::WebServerConfig( std::vector<block> blocks ) {
	for (std::vector<block>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
		servers.push_back(Server(*it));
	}
	size = servers.size();
}
		