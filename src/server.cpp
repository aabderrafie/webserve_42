#include "server.hpp"
#include <ctime>
#include <fstream> // Add this line

#include <limits.h>
#include <sys/wait.h>

// std::map<std::string, std::string> interpreters =  {
//     {".php", "/usr/bin/php-cgi"},
//     {".py", "/usr/bin/python3"},
//     {".sh", "/bin/bash"}
// };

std::string Request::execute_cgi(const std::string& interpreter , std::string root_cgi) 
{
    std::cout << "Executing CGI script: " << path << std::endl;
    // std::string uploaded_file;
    // if (isMultipart)
    // {
    //     response.upload_file(uploaded_file);
    //     std::cout << "Uploaded file: ok"  << std::endl;
    // }
    // std::cout << "interpreter" <<interpreter << std::endl;
    std::string path_ = root_cgi + this->path;
    // std::cout << "path_: " << path_ << std::endl;

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
    if (pid == 0) {  // Child process
std::stringstream ss;
ss << post_data.length();
std::string content_type = ss.str();
        close(pipefd[0]);  // Close unused read end
 std::vector<std::string> env_vars;

env_vars.push_back("REQUEST_METHOD=" + method);
env_vars.push_back("QUERY_STRING=" + query_string);
env_vars.push_back("CONTENT_TYPE=" + content_type);
env_vars.push_back("CONTENT_LENGTH=" + content_type);
env_vars.push_back("SCRIPT_FILENAME=" + path_);
env_vars.push_back("REDIRECT_STATUS=200"); // Required for PHP-CGI
// env_vars.push_back("UPLOADED_FILE=" + uploaded_file);
        std::vector<char*> envp;
        for (size_t i = 0; i < env_vars.size(); ++i)
            envp.push_back(const_cast<char*>(env_vars[i].c_str()));
        envp.push_back(NULL);

        if (method == "POST") {
            close(input_pipe[1]);  // Close write end in child
            dup2(input_pipe[0], STDIN_FILENO);
            close(input_pipe[0]);  // Close after dup2
        }
//   std::cout << "body: \n<p>" <<  body <<"<p> <br>"  << std::endl;
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // std::cout << "post_data: <p>" << std::endl;
        // std::cout<< post_data << "<p> <br>"; 
        // std::cout << "body: \n<p>" <<  body <<"<p> <br>"  << std::endl;
        std::vector<char*> args;
        args.push_back(const_cast<char*>(interpreter.c_str()));
        // std::string path_ = root_cgi + this->path;
        // std::cout << "path_: " << path_ << std::endl;
        args.push_back(const_cast<char*>(path_.c_str()));
        args.push_back(NULL);

        execve(interpreter.c_str(), args.data(), envp.data());
        perror("execve failed");
        exit(1);
    } 
    else if (pid > 0) {  // Parent process
        close(pipefd[1]);  // Close unused write end
        // std::cout << "Parent process" << std::endl;
        if (method == "POST") 
        {
            close(input_pipe[0]);  // Close read end

            size_t remaining = post_data.length();
            // std::cout << "Remaining: " << remaining << std::endl;
            const char* data_ptr = post_data.c_str();
            while (remaining > 0) {
                ssize_t result = write(input_pipe[1], data_ptr, remaining);
                if (result == -1) {
                    if (errno == EINTR) continue;  // Retry if interrupted by a signal
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
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
        while ((bytes_read = read(pipefd[0], buffer, BUFFER_SIZE - 1)) > 0) {
            buffer[bytes_read] = '\0';
            output += buffer;
        }
        close(pipefd[0]);

        waitpid(pid, NULL, 0);

        size_t header_end = output.find("\r\n\r\n");
        return (header_end != std::string::npos) ? output.substr(header_end + 4) : output;
    } 
    else {  // fork() failed
        std::cerr << "Fork failed" << std::endl;
        return "500 Internal Server Error\n";
    }
    // return "500 Internal Server Error\n";
}

std::string current_time() {
    std::time_t now = std::time(NULL);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

Server::Server() {
    std::cout << "Server created" << std::endl;
    // load_sessions_from_file(); // Add this line
}

Server::~Server() {}

// void Server::save_sessions_to_file() {
//     std::ofstream file("sessions.txt");
//     if (file.is_open()) {
//         // std::cout << "updating sessions file" << std::endl;
//         for (std::map<int, Session>::value_type& session : sessions) {
//             for (std::map<std::string, std::string>::value_type& data : session.second.session_data) {
//                 file << data.first << "=" << data.second << ";";
//             }
//         }
//         file.close();
//     } else {
//         std::cerr << "Unable to open sessions file for writing" << std::endl;
//     }
// }

// void Server::load_sessions_from_file() {
//     std::ifstream file("sessions.txt");
//     if (file.is_open()) {
//         int session_id;
//         std::cout << "loading sessions file.." << std::endl;
//         while (file >> session_id) {
//             std::cout << "session_id: " << session_id << std::endl;
//             Session session(session_id);
//             sessions.insert(std::make_pair(session_id, session));
//             session_id += 10;
//         }
//         file.close();
//     } else {
//         std::cerr << "Unable to open sessions file for reading" << std::endl;
//     }
// }

// void Server::load_sessions_from_file() {
//     std::ifstream file("sessions.txt");
//     if (file.is_open()) {
//         // std::cout << "loading sessions file.." << std::endl;
//         std::string line;
//         while (std::getline(file, line)) {
//             std::istringstream iss(line);
//             std::string token;
//             std::map<std::string, std::string> session_data;
//             while (std::getline(iss, token, ';')) {
//                 size_t pos = token.find("=");
//                 if (pos != std::string::npos) {
//                     std::string key = token.substr(0, pos);
//                     std::string value = token.substr(pos + 1);
//                     session_data.insert(std::make_pair(key, value));
//                 }
//             }
//         }
//         file.close();
//     } else {
//         std::cerr << "Unable to open sessions file for reading" << std::endl;
//     }
// }

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


bool isDirectory(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        std::cout << "Directory found" << std::endl;
        closedir(dir);
        return true;
    }
    return false;
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
            throw std::runtime_error("Error binding to port " + std::to_string(ntohs(server_addrs[i].sin_port)));

        if (listen(server_sockets[i], BACKLOG) < 0)
            throw std::runtime_error("Error listening on port " + std::to_string(ntohs(server_addrs[i].sin_port)));
    }
}


void Server::new_connection(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sin_size);
    if (client_socket < 0)
        throw std::runtime_error("Error accepting connection");
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
        throw std::runtime_error("Error receiving data from client");

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
//zouhir add this
#include <sstream> // Required for std::ostringstream

void Server::send_cgi(std::string extension, std::string path, int client_socket, Response& response)
{
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


// void Response::set_cookies(const std::string& cookies) {
//     Cookies = cookies;
//     std::cout << "Cookies: " << Cookies << std::endl;
// }

// void manageSessions(std::map<int, Session>& sessions, std::vector<std::string>& Cookies) {
//     int session_id = std::stoi(Cookies[0].substr(11));
//     if (sessions.find(session_id) == sessions.end()) {
//         std::cout << "Creating new session" << std::endl;
//         sessions.insert(std::make_pair(session_id, Session(session_id)));
//     }
// }

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

    std::string Cookies = response.request.getCookies();
    if (response.request.isInNeedOfCookies) {
        sessions.insert(std::make_pair(response.request.session_id, Session(response.request.session_id, Cookies)));
        for (std::map<std::string, std::string>::value_type& data : sessions[response.request.session_id].session_data) {
            std::cout << "hereee " << data.first << "=" << data.second << std::endl;
        }
    }
    // std::cout << "Cookies: " << Cookies << std::endl;
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
                            {poll_fds.erase(poll_fds.begin() + i);
                            i--;
                            }
                    }
            }
        }
}


