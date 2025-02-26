#include "server.hpp"
#include <ctime>
#include <fstream>


<<<<<<< HEAD

=======
std::string Request::execute_cgi(const std::string& interpreter , std::string root_cgi) 
{
  std::string path_ = root_cgi + this->path;
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return "500 Internal Server Error\n";
    }

    int input_pipe[2];  // Pipe for CGI input
    if (method == "POST" && pipe(input_pipe) == -1) {
        perror("pipe failed");
        return "500 Internal Server Error\n";
    }

    pid_t pid = fork();
    if (pid == 0)  // Child process
    { 
        close(pipefd[0]); 
        std::vector<std::string> env_vars ;
        std::ostringstream oss;
        oss << post_data.length();
        
        env_vars.push_back("REQUEST_METHOD=" + method);
        env_vars.push_back("QUERY_STRING=" + query_string);
        env_vars.push_back("CONTENT_TYPE=" + content_type);
        env_vars.push_back("CONTENT_LENGTH=" + oss.str());
        env_vars.push_back("SCRIPT_FILENAME=" + path_);
        env_vars.push_back("REDIRECT_STATUS=200");  // Required for PHP-CGI
    
        std::vector<char*> envp;
        for (size_t i = 0; i < env_vars.size(); ++i)
            envp.push_back(const_cast<char*>(env_vars[i].c_str()));
        envp.push_back(NULL);

        if (method == "POST") {
            close(input_pipe[1]);  // Close write end in child
            dup2(input_pipe[0], STDIN_FILENO);
            close(input_pipe[0]);  // Close after dup2
        }
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        std::vector<char*> args;
        args.push_back(const_cast<char*>(interpreter.c_str()));
        args.push_back(const_cast<char*>(path_.c_str()));
        args.push_back(NULL);
        execve(interpreter.c_str(), args.data(), envp.data());
        perror("execve failed");
        exit(1);
    } 
    else if (pid > 0) // Parent process
    {  
        close(pipefd[1]);  // Close unused write end
        std::cout << "Parent process" << std::endl;
        if (method == "POST") 
        {
            close(input_pipe[0]);  // Close read end
            size_t remaining = post_data.length();
            const char* data_ptr = post_data.c_str();
            while (remaining > 0) 
            {
                ssize_t result = write(input_pipe[1], data_ptr, remaining);
                if (result == -1) 
                {
                    if (errno == EINTR) continue;  // Retry if interrupted by a signal
                    if (errno == EAGAIN || errno == EWOULDBLOCK) 
                    {
                        usleep(1000);  // Small delay
                        continue;
                    }
                    perror("write failed");
                    break;
                }
                remaining -= result;
                data_ptr += result;
            }
            close(input_pipe[1]);  // Signal EOF to CGI
        }
        char buffer[BUFFER_SIZE];
        std::string output;
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, BUFFER_SIZE - 1)) > 0) 
        {
            buffer[bytes_read] = '\0';
            output += buffer;
        }
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
        size_t header_end = output.find("\r\n\r\n");
        return (header_end != std::string::npos) ? output.substr(header_end + 4) : output;
    } 
    else  // fork() failed
    { 
        std::cerr << "===>> Fork failed <<===" << std::endl;
        return "500 Internal Server Error\n";
    }
}
>>>>>>> 8f270e4 (waiting for taha ...)


std::string current_time() {
    std::time_t now = std::time(NULL);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

<<<<<<< HEAD
Server::Server() {}
=======
void Server::server_error(const std::string& message, int client_socket) {
   Response response(client_socket, *this);
    response.send_error_response(500, "text/html", error_pages[500]);
    throw std::runtime_error(message);  
    close(client_socket);
}

Server::Server() {
    std::cout << "Server created" << std::endl;
}
>>>>>>> 8f270e4 (waiting for taha ...)

Server::~Server() {}

void Server::server_init() {
    for (size_t i = 0; i < ports.size(); ++i) {
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
            throw std::runtime_error("Error creating socket");

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(ports[i]);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        memset(&(server_addr.sin_zero), 0, 8);

        struct pollfd server_poll_fd;
        server_poll_fd.fd = server_socket;
        server_poll_fd.events = POLLIN;

        poll_fds.push_back(server_poll_fd);
        server_sockets.push_back(server_socket);
        server_addrs.push_back(server_addr);
    }
}

std::vector<std::string> list_files(const std::string& directory) {
    std::vector<std::string> files;
    DIR* dirp = opendir(directory.c_str());
    struct dirent* dp;
    while ((dp = readdir(dirp)) != NULL)
        files.push_back(dp->d_name);
    closedir(dirp);
    return files;
}

void Server::bind_and_listen() {
    for (size_t i = 0; i < server_sockets.size(); ++i) {
        int opt = 1;
        if (setsockopt(server_sockets[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            throw std::runtime_error("Error setting socket options");

        if (bind(server_sockets[i], (struct sockaddr *)&server_addrs[i], sizeof(server_addrs[i])) < 0)
            throw std::runtime_error("Error binding to port " + tostring(ntohs(server_addrs[i].sin_port)));

        if (listen(server_sockets[i], BACKLOG) < 0)
            throw std::runtime_error("Error listening on port " + tostring(ntohs(server_addrs[i].sin_port)));
    }
}


void Server::new_connection(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sin_size);
    if (client_socket < 0)
        server_error("Error accepting connection", server_socket);
    struct pollfd client_poll_fd;
    client_poll_fd.fd = client_socket;
    client_poll_fd.events = POLLIN;
    poll_fds.push_back(client_poll_fd);

    std::cout << BLUE << "[" << current_time() << "] New client connected on socket " << client_socket << RESET << std::endl;
}


std::map<int, std::string> partial_requests ;

std::string Server::read_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE); 
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
    if (bytes_received < 0)
        server_error("Error receiving data", client_socket);

    if (bytes_received == 0) {
        std::string final_request = partial_requests[client_socket];
        partial_requests.erase(client_socket);
        return final_request;
    }

    buffer[bytes_received] = '\0';
    partial_requests[client_socket] += std::string(buffer, bytes_received);

    size_t header_end = partial_requests[client_socket].find("\r\n\r\n");
    if (header_end == std::string::npos) 
        return ""; 

    std::string headers = partial_requests[client_socket].substr(0, header_end + 4);
    size_t content_length_pos = headers.find("Content-Length: ");
    std::cout << "headers: " << headers << std::endl;
    std::cout << "content_length_pos: " << content_length_pos << std::endl;
    if (content_length_pos != std::string::npos) {
        size_t content_length_end = headers.find("\r\n", content_length_pos);
        std::cout << "content_length_end: " << content_length_end << std::endl;
        int content_length = std::atoi(headers.substr(content_length_pos + 16, content_length_end - content_length_pos - 16).c_str());
        size_t total_length = header_end + 4 + content_length;
        if (partial_requests[client_socket].size() < total_length)
            return "";
    }
    
    std::string full_request = partial_requests[client_socket];
    partial_requests.erase(client_socket);
    return full_request;
}

bool Server::check_method(const std::string& method, const std::vector<std::string>& allowed_methods) {
    return std::find(allowed_methods.begin(), allowed_methods.end(), method) != allowed_methods.end();
}

//zouhir add this
bool Server::is_cgi(std::string path,std::string &extension)
{
    size_t dot_pos = path.find_last_of('.');
    if(dot_pos < path.length())
    {
        extension = path.substr(dot_pos);
        std::ifstream file((this->locations["/cgi-bin"].root + path).c_str(), std::ios::binary);
        return (file.is_open()&&this->locations["/cgi-bin"].cgi.find(extension) != this->locations["/cgi-bin"].cgi.end());
    }
    return false;
}
<<<<<<< HEAD
//zouhir add this
=======


>>>>>>> 8f270e4 (waiting for taha ...)
void Server::send_cgi(std::string extension, std::string path, int client_socket, Response& response)
{
if (std::find(this->locations["/cgi-bin"].allowed_methods.begin(),
              this->locations["/cgi-bin"].allowed_methods.end(),
              response.request.getMethod()) == this->locations["/cgi-bin"].allowed_methods.end()) {
    return response.send_error_response(405, "text/html", error_pages[405]);
}
    // std::cout << this->locations["/cgi-bin"].allowed_methods[0] << std::endl;
  
    std::string interpreter = this->locations["/cgi-bin"].cgi[extension];
    std::string script_path = this->locations["/cgi-bin"].root + path;
    std::string cgi_output = response.request.execute_cgi(interpreter, this->locations["/cgi-bin"].root);

    // Convert size_t to string using std::ostringstream (C++98 compatible)
    std::ostringstream oss;
    oss << cgi_output.length();
    std::string content_length = oss.str();

    std::string response_ = response.request.getHttpVersion() + 
        " 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + 
        content_length + "\r\n\r\n" + cgi_output;

    send(client_socket, response_.c_str(), response_.length(), 0);
}

bool Server::handle_client(int client_socket) {

    std::string body = read_request(client_socket);
    if (body.empty())
        return false;
    Response response(client_socket, *this);
    response.request = Request(body);
    std::string method = response.request.getMethod();
    std::string uri = response.request.getPath();
    std::string root_uri = locations[uri].root;
    std::string path = root_uri + uri;

    if(!check_method(method, locations["/"].allowed_methods)) 
        return response.send_error_response(405, "text/html", error_pages[405]) , close(client_socket), true;

    std::cout << YELLOW << "[" << current_time() << "] Request method: " << method << ", Path: " << path << RESET << std::endl;

    if (isDirectory(path)) {
        if (!check_method(method, locations[uri].allowed_methods)) 
            return response.send_error_response(405, "text/html", error_pages[405]), true;
        if (locations[uri].directory_listing)
            return response.list_directory_contents(path), true;
        else {
            if (!path.empty() && path[path.length() - 1] != '/')
                path += '/';
            path += locations[uri].default_file;
        }
    }
    else {
        std::string extension;
        std::string default_file;
        if (is_cgi(uri,extension)) 
            default_file = locations["/cgi-bin"].root + path;
        else 
            default_file = locations["/"].root + path;
        path = default_file;
    }

 if(!response.check_error(path))
        return true;

    std::string extension;
    if (is_cgi(uri,extension))
        send_cgi(extension, uri, client_socket, response);
    else
    {
          if (method == "GET")
        response.handle_get_request(path);
    else if (method == "POST")
        response.handle_post_request(path);
     else if (method == "DELETE") 
        response.handle_delete_request();
     else 
        response.send_error_response(405, "text/html", error_pages[405]);
    }
    
    partial_requests.erase(client_socket);
    close(client_socket);
    return true;
}

void Server::start_server() {

        int poll_count = poll(poll_fds.data(), poll_fds.size(),0);
        if (poll_count < 0)
            throw std::runtime_error("Error polling for events");
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (std::find(server_sockets.begin(), server_sockets.end(), poll_fds[i].fd) != server_sockets.end())
                    new_connection(poll_fds[i].fd);
                else 
                    {
                        if(handle_client(poll_fds[i].fd))
                            poll_fds.erase(poll_fds.begin() + i) , i--;
                    }
            }
        }
}


