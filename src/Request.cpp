#include "Request.hpp"


Request::Request(const string &body)
{
    isMultipart = false;
    isUrlEncoded = false;
    if(body.find("Content-Type: application/x-www-form-urlencoded") != std::string::npos)
        isUrlEncoded = true;

    else if (body.find("Content-Type: multipart/form-data") != std::string::npos)
        isMultipart = true;

    std::istringstream stream(body);
    
    std::string line;
    bool isBody = false;
    bool isFirstLine = true;
    while (std::getline(stream, line)) {

        if (line[0] == 13) {
            if (!isBody) {
                isBody = true;
                continue;
            }
        }
        if (!isBody) {
            if (isFirstLine) {
                parseRequestLine(line);
                isFirstLine = false;
            } 
            else if (line.find(": ") != std::string::npos) 
                 parseHeader(line);
        }
        else if(isUrlEncoded) 
            parseUrlEncodedData(line);
    }


    size_t method_end = body.find(' ');
    method = body.substr(0, method_end);

    size_t path_start = method_end + 1;
    size_t path_end = body.find(' ', path_start);
    std::string full_path = body.substr(path_start, path_end - path_start);

    size_t query_start = full_path.find('?');
    if (query_start != std::string::npos) {
        path = full_path.substr(0, query_start);
        query_string = full_path.substr(query_start + 1);
    } else {
        path = full_path;
        query_string = "";
    }

    size_t content_type_pos = body.find("Content-Type: ");
    if (content_type_pos != std::string::npos) 
    {
        size_t type_start = content_type_pos + 14;
        size_t type_end = body.find("\r\n", type_start);
        content_type = body.substr(type_start, type_end - type_start);

        if (content_type.find("multipart/form-data") != std::string::npos) {
            size_t boundary_pos = content_type.find("boundary=");
            if (boundary_pos != std::string::npos) {
                boundary = "--" + content_type.substr(boundary_pos + 9);
            }
        }
    }

    if (method == "POST") {
        size_t header_end = body.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            post_data = body.substr(header_end + 4);
        }
    }
}

std::string Request::trim(std::string str) {
    size_t first = str.find_first_not_of(" \t\n\r", 0);
    size_t last = str.find_last_not_of(" \t\n\r", str.length());
    if (first == std::string::npos) str = "";
    else str = str.substr(first, (last - first + 1));
    return str;
}

void Request::parseRequestLine(const string& line) {
    std::istringstream iss(line);
    iss >> method >> path >> httpVersion;

}

void Request::parseHeader(const string& line) {
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        headers[key] = value;
    }
}

void Request::parseUrlEncodedData(const string& body) {
    std::istringstream iss(body);
    std::string pair;
    while (std::getline(iss, pair, '&')) {
        size_t equalPos = pair.find('=');
        if (equalPos != std::string::npos) {
            std::string key = pair.substr(0, equalPos);
            std::string value = pair.substr(equalPos + 1);
            formData[key] = value;
        }
    }
}

int Request::getContentLength() {
    if(headers.find("Content-Length") != headers.end())
       //return std::stoi(headers["Content-Length"]);
        return std::atoi(headers["Content-Length"].c_str());
    return -1;
}



std::string Request::execute_cgi(const std::string& interpreter , std::string root_cgi) 
{
        Message("Executing CGI script: " + interpreter, MAGENTA);

  std::string path_ = root_cgi + this->path;
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return "500 Internal Server Error\n";
    }
    int input_pipe[2];  
    if (method == "POST" && pipe(input_pipe) == -1) {
        perror("pipe failed");
        return "500 Internal Server Error\n";
    }
    pid_t pid = fork();
    if (pid == 0)  
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
        env_vars.push_back("REDIRECT_STATUS=200");  
    
        std::vector<char*> envp;
        for (size_t i = 0; i < env_vars.size(); ++i)
            envp.push_back(const_cast<char*>(env_vars[i].c_str()));
        envp.push_back(NULL);

        if (method == "POST") {
            close(input_pipe[1]);  
            dup2(input_pipe[0], STDIN_FILENO);
            close(input_pipe[0]);  
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
    else if (pid > 0) 
    {  
        close(pipefd[1]);  
        if (method == "POST") 
        {
            close(input_pipe[0]); 
            size_t remaining = post_data.length();
            const char* data_ptr = post_data.c_str();
            while (remaining > 0) 
            {
                ssize_t result = write(input_pipe[1], data_ptr, remaining);
                if (result == -1) 
                {
                    if (errno == EINTR) continue;  
                    if (errno == EAGAIN || errno == EWOULDBLOCK) 
                    {
                        usleep(1000); 
                        continue;
                    }
                    perror("write failed");
                    break;
                }
                remaining -= result;
                data_ptr += result;
            }
            close(input_pipe[1]);  
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
    else 
    { 
        Message("fork failed", RED);
        return "500 Internal Server Error\n";
    }
}