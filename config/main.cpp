#include "parsing.hpp"

int	main(void)
{
	try {
		std::string configContent = readFile("/Users/tlasfar/Webserv/config/full.conf");
		parser p; WebServerConfig conf = p.parse(configContent);
		// std::cout << "Servers: " << conf.servers.size() << std::endl;
		// for (std::vector<WebServerConfig::Server>::iterator it = conf.servers.begin(); it < conf.servers.end(); it++) {
		// 	std::cout << "Server: " << it->host << ":" << it->port << std::endl;
		// 	std::cout << "Server names: ";
		// 	for (std::vector<std::string>::iterator it2 = it->server_names.begin(); it2 < it->server_names.end(); it2++) {
		// 		std::cout << *it2 << " ";
		// 	}
		// 	std::cout << std::endl;
		// 	std::cout << "Is default server: " << it->is_default_server << std::endl;
		// 	std::cout << "Error pages: ";
		// 	for (std::map<int, std::string>::iterator it2 = it->error_pages.begin(); it2 != it->error_pages.end(); it2++) {
		// 		std::cout << it2->first << " -> " << it2->second << " ";
		// 	}
		// 	std::cout << std::endl;
		// 	std::cout << "Client max body size: " << it->client_max_body_size << std::endl;
		// 	std::cout << "    Routes: " << it->routes.size() << std::endl;
		// 	for (std::vector<WebServerConfig::Server::Route>::iterator it2 = it->routes.begin(); it2 < it->routes.end(); it2++) {
		// 		std::cout << "    Route: " << it2->path << std::endl;
		// 		std::cout << "\tAllowed methods: ";
		// 		for (std::vector<std::string>::iterator it3 = it2->allowed_methods.begin(); it3 < it2->allowed_methods.end(); it3++) {
		// 			std::cout << *it3 << "\t ";
		// 		}
		// 		std::cout << std::endl;
		// 		std::cout << "\tRedirect: " << it2->redirect << std::endl;
		// 		std::cout << "\tRoot directory: " << it2->root_directory << std::endl;
		// 		std::cout << "\tDirectory listing: " << it2->directory_listing << std::endl;
		// 		std::cout << "\tDefault file: " << it2->default_file << std::endl;
		// 		std::cout << "\tCGI extension: " << it2->cgi_extension << std::endl;
		// 		std::cout << "\tUpload path: " << it2->upload_path << std::endl;
		// 	}
		// }



		WebServerConfig::Server tmp = conf.getServer(0);
		std::cout << "Servers: " << conf.size << std::endl;
		std::cout << "Server: " << tmp.host << ":" << tmp.port << std::endl;	
		std::cout << "Server names: ";
		for (std::vector<std::string>::iterator it = tmp.server_names.begin(); it != tmp.server_names.end(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;
		std::cout << "Is default server: " << tmp.is_default_server << std::endl;
		std::cout << "Error pages: ";
		for (std::map<int, std::string>::iterator it = tmp.error_pages.begin(); it != tmp.error_pages.end(); ++it) {
			std::cout << it->first << " -> " << it->second << " ";
		}
		std::cout << std::endl;
		std::cout << "Client max body size: " << tmp.client_max_body_size << std::endl;
		std::cout << "Routes: " << tmp.routes.size() << std::endl;
		for (std::vector<WebServerConfig::Server::Route>::iterator it = tmp.routes.begin(); it != tmp.routes.end(); ++it) {
			std::cout << "    Route: " << it->path << std::endl;
			std::cout << "\tAllowed methods: ";
			for (std::vector<std::string>::iterator it2 = it->allowed_methods.begin(); it2 != it->allowed_methods.end(); ++it2) {
				std::cout << *it2 << " ";
			}
			std::cout << std::endl;
			std::cout << "\tRedirect: " << it->redirect << std::endl;
			std::cout << "\tRoot directory: " << it->root_directory << std::endl;
			std::cout << "\tDirectory listing: " << it->directory_listing << std::endl;
			std::cout << "\tDefault file: " << it->default_file << std::endl;
			std::cout << "\tCGI extension: " << it->cgi_extension << std::endl;
			std::cout << "\tUpload path: " << it->upload_path << std::endl;
		}

	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}