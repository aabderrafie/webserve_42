#include "server.hpp"
#include <ctime>

#include <limits.h>
#include <sys/wait.h>

std::map<std::string, std::string> interpreters = {
    {".php", "/usr/bin/php-cgi"},
    {".py", "/usr/bin/python3"},
    {".sh", "/bin/bash"}
};




void parse_multipart_form_data(const std::string& post_data, const std::string& boundary, std::string& uploaded_file_path) 
{

    size_t pos = 0;
    while ((pos = post_data.find(boundary, pos)) != std::string::npos) {
        pos += boundary.length();
        size_t name_pos = post_data.find("name=\"", pos);
        if (name_pos == std::string::npos) break;
        
        size_t name_end = post_data.find("\"", name_pos + 6);
        std::string field_name = post_data.substr(name_pos + 6, name_end - name_pos - 6);
        
        size_t filename_pos = post_data.find("filename=\"", name_end);
        if (filename_pos != std::string::npos) {
            size_t filename_end = post_data.find("\"", filename_pos + 10);
            std::string filename = post_data.substr(filename_pos + 10, filename_end - filename_pos - 10);

            size_t file_start = post_data.find("\r\n\r\n", filename_end) + 4;
            size_t file_end = post_data.find(boundary, file_start) - 2;
            std::string file_content = post_data.substr(file_start, file_end - file_start);

            uploaded_file_path = "./files/html/uploads/" + filename;
            std::ofstream out_file(uploaded_file_path.c_str(), std::ios::binary);
            if (out_file) {
                out_file.write(file_content.c_str(), file_content.size());
                out_file.close();
                std::cout << "Saved file: " << uploaded_file_path << std::endl;
            } else {
                std::cerr << "Failed to save file: " << uploaded_file_path << std::endl;
            }
        }
    }
}

// void parse_multipart_form_data(const std::string& post_data, const std::string& boundary, std::string& uploaded_file_path) {
//     size_t pos = 0;
//     while ((pos = post_data.find(boundary, pos)) != std::string::npos) {
//         pos += boundary.length();
//         size_t name_pos = post_data.find("name=\"", pos);
//         if (name_pos == std::string::npos) break;

//         size_t name_end = post_data.find("\"", name_pos + 6);
//         std::string field_name = post_data.substr(name_pos + 6, name_end - name_pos - 6);

//         size_t filename_pos = post_data.find("filename=\"", name_end);
//         if (filename_pos != std::string::npos) {
//             size_t filename_end = post_data.find("\"", filename_pos + 10);
//             std::string filename = post_data.substr(filename_pos + 10, filename_end - filename_pos - 10);

//             size_t file_start = post_data.find("\r\n\r\n", filename_end) + 4;
//             size_t file_end = post_data.find(boundary, file_start) - 2;

//             uploaded_file_path = "./files/html/uploads/" + filename;
//             std::ofstream out_file(uploaded_file_path.c_str(), std::ios::binary);
//             if (out_file) {
//                 out_file.write(post_data.data() + file_start, file_end - file_start);
//                 out_file.close();
//                 std::cout << "Saved file: " << uploaded_file_path << std::endl;
//             } else {
//                 std::cerr << "Failed to save file: " << uploaded_file_path << std::endl;
//             }
//         }
//     }
// }


// Set a file descriptor to non-blocking mode
void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL) failed");
        exit(1);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL) failed");
        exit(1);
    }
}
std::string execute_cgi(const std::string& script_path, const std::string& interpreter,
                        const std::string& method, const std::string& query_string,
                        const std::string& post_data, const std::string& content_type,
                        const std::string& boundary ) 
{

    std::cout << "Executing CGI script: " << script_path << std::endl;

    std::string uploaded_file;
    if (content_type.find("multipart/form-data") != std::string::npos) {
        parse_multipart_form_data(post_data, boundary, uploaded_file);
    }
    std::cout << "Uploaded file: ok"  << std::endl;

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
        // Set pipes to non-blocking mode
    set_non_blocking(input_pipe[1]);
    set_non_blocking(pipefd[0]);

    pid_t pid = fork();
      if (pid == 0) {  // Child process
        close(pipefd[0]);  // Close unused read end
        std::vector<std::string> env_vars = {
            "REQUEST_METHOD=" + method,
            "QUERY_STRING=" + query_string,
            "CONTENT_TYPE=" + content_type,
            "CONTENT_LENGTH=" + std::to_string(post_data.length()),
            "SCRIPT_FILENAME=" +script_path,
            "REDIRECT_STATUS=200",  // Required for PHP-CGI
            "UPLOADED_FILE=" + uploaded_file
        };

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
        
        args.push_back(const_cast<char*>(script_path.c_str()));
        args.push_back(NULL);

    
   



        execve(interpreter.c_str(), args.data(), envp.data());
        perror("execve failed");
        exit(1);
    }  else if (pid > 0) {  // Parent process
        close(pipefd[1]);  // Close unused write end

        if (method == "POST") {
            close(input_pipe[0]);  // Close read end
            size_t remaining = post_data.length();
            const char* data_ptr = post_data.c_str();
            while (remaining > 0) {
                ssize_t result = write(input_pipe[1], data_ptr, std::min(remaining, static_cast<size_t>(BUFFER_SIZE)));
                if (result == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        usleep(1000);
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
        while (true) {
            bytes_read = read(pipefd[0], buffer, BUFFER_SIZE - 1);
            if (bytes_read == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    usleep(1000);
                    continue;
                }
                perror("read failed");
                break;
            } else if (bytes_read == 0) {
                break;
            }
            buffer[bytes_read] = '\0';
            output += buffer;
        }
        close(pipefd[0]);

        waitpid(pid, NULL, 0);

        size_t header_end = output.find("\r\n\r\n");
        return (header_end != std::string::npos) ? output.substr(header_end + 4) : output;
    } else {  // fork() failed
        std::cerr << "Fork failed" << std::endl;
        return "500 Internal Server Error\n";
    }
    // return "500 Internal Server Error\n";
}


std::string current_time() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

Server::Server()  {}

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
Server::~Server() {
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


// std::string Server::read_request(int client_socket) {
//     char buffer[BUFFER_SIZE];
//     std::string body;
//     int bytes_received;

//     while (true) {
//         bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
//         if (bytes_received < 0) 
//             throw std::runtime_error("Error receiving data");
//         else if (bytes_received == 0) 
//             break;
//         buffer[bytes_received] = '\0';
//         body += std::string(buffer, bytes_received);
//         if (bytes_received < BUFFER_SIZE - 1) 
//             break;
//     }

//     return body;
// }

std::string Server::read_request(int client_socket) 
{
    std::vector<char> buffer(BUFFER_SIZE);
    std::string body;
    ssize_t bytes_received;

    while (true) {
        std::cout << "Request: reading" << std::endl;
        bytes_received = recv(client_socket, buffer.data(), buffer.size(), 0);

        if (bytes_received < 0) {
            // Handle specific errors (e.g., EAGAIN/EWOULDBLOCK for non-blocking sockets)
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available (non-blocking socket)
                break;
            } else {
                // Other errors
                throw std::runtime_error("Error receiving data");
            }
        } else if (bytes_received == 0) {
            // Connection closed by client
            break;
        }

        // Append received data to the body
        body.append(buffer.data(), bytes_received);

        // Check if we've reached the end of the request
        // For HTTP requests, the end is indicated by double CRLF (\r\n\r\n)
        if (body.find("\r\n\r\n") != std::string::npos) {
            // For POST requests, check Content-Length to ensure the entire body is read
            size_t content_length_pos = body.find("Content-Length: ");
            if (content_length_pos != std::string::npos) {
                size_t content_length_end = body.find("\r\n", content_length_pos);
                std::string content_length_str = body.substr(
                    content_length_pos + 16, // "Content-Length: " is 16 characters
                    content_length_end - (content_length_pos + 16)
                );
                size_t content_length = std::stoul(content_length_str);

                // Find the start of the body (after headers)
                size_t header_end = body.find("\r\n\r\n");
                size_t body_start = header_end + 4; // +4 for \r\n\r\n

                // Calculate how much more data to read
                size_t body_received = body.size() - body_start;
                if (body_received < content_length) {
                    // Continue reading until the entire body is received
                    continue;
                }
            }
            break;
        }
    }

    std::cout << "Request: ok " <<  std::endl;  
    // std::cout <<" body: \n" << body <<"|\n" <<std::endl;
    return body;
}

void Server::handle_client(int client_socket) {

    Response response(client_socket, *this);
    std::string body = read_request(client_socket);
    if(body.empty())
    {
        std::cout << RED<< "Empty request" << std::endl;
        close(client_socket);
        return;
    }
    response.request = Request(body);

    std::string method = response.request.getMethod();
    std::string path = response.request.getPath();
    // std::string httpVersion = response.request.getHttpVersion();
    std::string post_data = response.request.getPostData();
    std::string query_string = response.request.getQueryString();
    std::string content_type = response.request.getContentType();
    std::string boundary = response.request.getBoundary();
     Server server1;
     std::cout <<"path"<< path << std::endl;
// still wating for the request parsing will done by zouhir 
    std::cout << YELLOW << "[" << current_time() << "] Request method: " << method << RESET << std::endl;

/////////////////////////////////////////  CGI  //////////////////////////////////////////
    size_t dot_pos = path.find_last_of('.');


    if (dot_pos != std::string::npos)
        {
            // std::cout<< "\n\n"<< "dot_pos 2 "<<dot_pos<<"\n\n";
            std::string extension = path.substr(dot_pos);
            // std::cout<< "\n\n"<< "extension"<<extension<<"\n\n";
            if (interpreters.find(extension) != interpreters.end())
            {       
                std::string interpreter = interpreters[extension];
                std::string script_path = "." + server1.root_location.root + path;
                std::string cgi_output = execute_cgi(script_path, interpreter, method, query_string, post_data,content_type,boundary) ;
                // Send the CGI output as the HTTP response
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(cgi_output.length()) + "\r\n\r\n" + cgi_output;
                send(client_socket, response.c_str(), response.length(), 0);
            }
            else
            {
                if (method == "GET")
                    response.handle_get_request(body);
                else if (method == "POST")
                    response.handle_post_request( body);
                else if(method == "DELETE")
                    response.handle_delete_request(body);
                else
                    response.send_error_response(405, "text/html",  error_pages[405]);
            }
        }
        else
        {
            if (method == "GET")
                response.handle_get_request(body);
            else if (method == "POST")
                response.handle_post_request( body);
            else if(method == "DELETE")
                response.handle_delete_request(body);
            else
                response.send_error_response(405, "text/html",  error_pages[405]);
        }





    close(client_socket);
}



void Server::start_server() {

        int poll_count = poll(poll_fds.data(), poll_fds.size(), 100);
        if (poll_count < 0)
            throw std::runtime_error("Error polling for events");
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (std::find(server_sockets.begin(), server_sockets.end(), poll_fds[i].fd) != server_sockets.end())
                    new_connection(poll_fds[i].fd);
                else {
                    handle_client(poll_fds[i].fd);
                    poll_fds.erase(poll_fds.begin() + i);
                    --i;
                }
            }
        }
}