#include "parsing.hpp"

parser::parser() {}

parser::~parser() {}

static void handleDirective(block& ret, std::vector<Token>::iterator& current) {
	std::string currentDirective = current->value;
	while ((++current)->type != "symbol") {
		if (current->type == "value") {
			ret.directives[currentDirective].push_back(current->value);
		} else if (current->type == "directive") {
				throw std::runtime_error("Error: expected semicolon " + current->value);
		} else if ((current+1)->value == ";")
			break;
	}
}

block parser::parseBlock(std::vector<Token>::iterator& current, std::vector<Token>::iterator end) {
	block	ret;
	int		i;

	i = 0;
	if (current->value == "server")
		ret.block_name = current->value;
	else {
		ret.block_name = (++current)->value;
		if (current->value == "{")
			throw std::runtime_error("Error: expected location name");
	}
	++current;
	if (current->value != "{")
		throw std::runtime_error("Error: missing opening bracket");
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

void printBlock(block& ret) {
	std::cout << "block name: " << ret.block_name << std::endl;
	for (std::map<std::string, std::vector<std::string> >::iterator it = ret.directives.begin(); it != ret.directives.end(); ++it) {
		std::cout << "   -" << it->first << " ";
		std::cout << "number of argument " << it->second.size() << ": \n";
		for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 < it->second.end(); ++it2) {
			std::cout << "\tvalue: " << *it2 << std::endl;
		}
	} for (std::vector<block>::iterator it = ret.recursive_blocks.begin(); it != ret.recursive_blocks.end(); ++it) {
		std::cout << "\n-recursive block name: " << it->block_name << std::endl;
		for (std::map<std::string, std::vector<std::string> >::iterator it2 = it->directives.begin(); it2 != it->directives.end(); ++it2) {
			std::cout << "    " << it2->first << std::endl;
			for (std::vector<std::string>::iterator it3 = it2->second.begin(); it3 < it2->second.end(); ++it3) {
				std::cout << "\tvalue: " << *it3 << std::endl;
			}
		}
	}
}

void parser::parse(const std::string &path) {
	std::string configContent = readFile(path);
	Tokenizer	tok;
	tok.tokenize(configContent);
	std::vector<Token> tokens = tok.getTokens();
	std::vector<Token>::iterator it = tokens.begin();
	while (it < tokens.end()) {
		if (it->type == "block" && it->value == "server") {
			block tmp = parseBlock(it, tokens.end());
			blocks.push_back(tmp);
		}

		else throw std::runtime_error("Error: unautorized token: " + it->value);
		it++;
	}
	std::vector<block> ret = getConfig();
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

static bool isValidExt(std::string ref, int &i) {
	if (i % 2 == 0) {
		if (ref[0] != '.')
			return false;
	} else {
		if (!isValidPath(ref))
			return false;
	}
	return true;
}

void configureLocation( block& ref, Location& loc ) {
	for (std::map<std::string, std::vector<std::string> >::iterator it2 = ref.directives.begin(); it2 != ref.directives.end(); ++it2) {
		if (it2->first == "allowed_methods") {
			if (!validateAllowedMethods(it2->second))
				throw std::runtime_error("Error: allowed_methods: invalid argument");
			loc.allowed_methods.erase(loc.allowed_methods.begin(), loc.allowed_methods.end());
			loc.allowed_methods = it2->second;
		} else if (it2->first == "root_directory") {
			if (it2->second.size() > 1 || !isValidPath(*it2->second.begin()))
				throw std::runtime_error("Error: root_directory: invalid argument");
			loc.root.erase(loc.root.begin(), loc.root.end());
			loc.root = *it2->second.begin();
		} else if (it2->first == "directory_listing") {
			if (it2->second.size() > 1)
				throw std::runtime_error("Error: directory_listing: invalid argument");
			if (*it2->second.begin() == "on") loc.directory_listing = true;
			else if (*it2->second.begin() == "off") loc.directory_listing = false;
			else throw std::runtime_error("Error: directory_listing: invalid argument");
		} else if (it2->first == "cgi") {
			int i = 0;
			std::string save;
			for (std::vector<std::string>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
				if (!isValidExt(*it3, i))
					throw std::runtime_error("Error: cgi_extensions: invalid argument");
				if (i % 2 == 0)
					loc.cgi[*it3] = "";
				else
					loc.cgi[save] = *it3;
				save = *it3;
				i++;
			}
			if (i % 2 != 0)
				throw std::runtime_error("Error: cgi: invalid argument");
 		} else if (it2->first == "default_file") {
			if (it2->second.size() > 1 || !isValidPath(*it2->second.begin()))
				throw std::runtime_error("Error: default_file: invalid argument");
			loc.default_file.erase(loc.default_file.begin(), loc.default_file.end());
			loc.default_file = *it2->second.begin();
		}
	}
}

static bool validateHost(std::string input) {
	if (input.find(":") != std::string::npos)
		return false;
	if (input == "localhost")
		return true;
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
		size_t digitCount = 0;
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

// static bool validateServerNames(const std::vector<std::string>& serverNames) {
// 	for (std::vector<std::string>::const_iterator it = serverNames.begin(); it != serverNames.end(); ++it) {
// 		if (!isValidServerName(*it))
// 			return false;
// 	} return true;
// }

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

static bool validateClientMaxBodySize( std::string ref ,int *digitCount) {
	std::string size = ref.substr(0, ref.size() - 1);
	int tmp = std::stoi(size);
	while (tmp != 0) {
		tmp /= 10;
		(*digitCount)++;
	}
	std::string unit = ref.substr(*digitCount);
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

Server configureServer( block& ref ) {
	Server serv;
	serv.host = "127.0.0.1";
	serv.server_name = "localhost";
	serv.client_max_body_size = 1024 * 1024;
	serv.ports.push_back(8081);
	for (std::map<std::string, std::vector<std::string> >::iterator it2 = ref.directives.begin(); it2 != ref.directives.end(); ++it2) {
		if (it2->first == "server_name") {
			if (it2->second.size() > 1)
				throw std::runtime_error("Error: too many argument for server_name");
			else if (isValidServerName(*it2->second.begin()))
				serv.server_name = *it2->second.begin();
		} else if (it2->first == "listen") {
			if (it2->second.size() == 1) {
				if (it2->second[0].find(":")) {
					std::vector<std::string> listen = split(*it2->second.begin(), ':');
					if (it2->second.size() == 2) {
						if (!validateHost(listen[0]) || !validatePort(listen[1]))
							throw std::runtime_error("Error: listen: invalid argument");
						serv.host = listen[0];
						serv.ports.push_back(std::stoi(listen[1]));
					} else if (!validateHost(*it2->second.begin())) {
						serv.host = *it2->second.begin();
					} else
						throw std::runtime_error("Error: listen: invalid argument");
				} else if (it2->second.size() == 2) {
					if (!validateHost(*it2->second.begin()) || !validatePort(it2->second[1]))
						throw std::runtime_error("Error: listen: invalid argument");
					serv.host = *it2->second.begin();
					serv.ports.push_back(std::stoi(it2->second[1]));
				}
			}
		} else if (it2->first == "host") {
			if (!validateHost(*it2->second.begin()))
				throw std::runtime_error("Error: host: invalid argument");
			serv.host = *it2->second.begin();
		} else if (it2->first == "port") {
			serv.ports.erase(serv.ports.begin(), serv.ports.end());
			for (std::vector<std::string>::iterator port_it = it2->second.begin(); port_it < it2->second.end(); port_it++) {
				if (!validatePort(*port_it))
					throw std::runtime_error("Error: port: invalid argument");
				serv.ports.push_back(std::stoi(*port_it));
			}
		} else if (it2->first == "error_page") { 
			std::vector<std::string> error_page = it2->second;
			if (!validateErrorPages(error_page))
				throw std::runtime_error("Error: error_page: invalid argument");
			size_t i = 0;
			while (i < error_page.size()) {
				serv.error_pages[std::stoi(error_page[i])] = error_page[(i+1)];
				i += 2;
			}
		} else if (it2->first == "client_max_body_size") {
			int digitCount = 0;
			if (!validateClientMaxBodySize(*it2->second.begin(), &digitCount))
				throw std::runtime_error("Error: client_max_body_size: invalid argument");
			serv.client_max_body_size = std::stoi(*it2->second.begin());
			std::string unit = it2->second[0].substr(digitCount);
			if (unit == "K")
				serv.client_max_body_size *= 1024;
			else if (unit == "M")
				serv.client_max_body_size *= 1024 * 1024;
		}
	}
	for (std::vector<block>::iterator it2 = ref.recursive_blocks.begin(); it2 != ref.recursive_blocks.end(); ++it2) {
			configureLocation(*it2, serv.locations[it2->block_name]);
	}
	return serv;
}

std::vector<Server> initConfig( std::vector<block> blocks ) {
	std::vector<Server> servers;
	for (std::vector<block>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
		Server serv;
		servers.push_back(configureServer(*it));
	}
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
		if (it->locations.find("/") == it->locations.end()) {
			Location loc;
			loc.directory_listing = false;
			it->locations["/"] = loc;
		} 
		if (it->locations["/"].root.empty()) {
			it->locations["/"].root = "./file/html";
		} if (it->locations["/"].default_file.empty()) {
			it->locations["/"].default_file = "index.html";
		} if (it->locations["/"].allowed_methods.empty()) {
			it->locations["/"].allowed_methods.push_back("GET");
		} if (it->locations["/"].cgi.empty()) {
			it->locations["/"].cgi[".php"] = "/usr/bin/php";
		}
		if (it->locations.find("/upload") == it->locations.end()) {
			Location loc;
			loc.directory_listing = false;
			it->locations["/upload"] = loc;
		}
		if (it->locations["/upload"].root.empty()) {
			it->locations["/upload"].root = "./file/html/upload";
		} if (it->locations["/upload"].default_file.empty()) {
			it->locations["/upload"].default_file = "index.html";
		} if (it->locations["/upload"].allowed_methods.empty()) {
			it->locations["/upload"].allowed_methods.push_back("GET");
			it->locations["/upload"].allowed_methods.push_back("POST");
		} if (it->locations["/upload"].cgi.empty()) {
			it->locations["/upload"].cgi[".php"] = "/usr/bin/php";
			it->locations["/upload"].cgi[".py"] = "/usr/bin/python3";
		}
		if (it->locations.find("/cgi-bin") == it->locations.end()) {
			Location loc;
			loc.directory_listing = false;
			it->locations["/cgi-bin"] = loc;
		}
		if (it->locations["/cgi-bin"].root.empty()) {
			it->locations["/cgi-bin"].root = "./file/html/cgi-bin";
		} if (it->locations["/cgi-bin"].default_file.empty()) {
			it->locations["/cgi-bin"].default_file = "index.html";
		} if (it->locations["/cgi-bin"].allowed_methods.empty()) {
			it->locations["/cgi-bin"].allowed_methods.push_back("GET");
			it->locations["/cgi-bin"].allowed_methods.push_back("POST");
		} if (it->locations["/cgi-bin"].cgi.empty()) {
			it->locations["/upload"].cgi[".php"] = "/usr/bin/php";
			it->locations["/upload"].cgi[".py"] = "/usr/bin/python3";
		}
	}
	return servers;
}

