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


std::string Request::execute_cgi(const std::string& interpreter, std::string root_cgi) 
{

    Message("Executing CGI script: " + interpreter, MAGENTA);

    std::string path_ = root_cgi + this->path;

    std::ostringstream input_filename_oss;
    std::ostringstream output_filename_oss;
    input_filename_oss << "/tmp/cgi_input_" << time(NULL) << "_";
    output_filename_oss << "/tmp/cgi_output_" << time(NULL) << "_";

    std::string input_filename = input_filename_oss.str();
    std::string output_filename = output_filename_oss.str();

    int input_fd = open(input_filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (input_fd == -1) 
    {
        perror("Failed to create input file");
        return "500 Internal Server Error\n";
    }

    int output_fd = open(output_filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) 
    {
        perror("Failed to create output file");
        close(input_fd);
        return "500 Internal Server Error\n";
    }

    if (method == "POST") 
    {
        ssize_t bytes_written = write(input_fd, post_data.c_str(), post_data.length());
        if (bytes_written == -1) 
        {
            perror("Failed to write POST data to input file");
            close(input_fd);
            close(output_fd);
            return "500 Internal Server Error\n";
        }
    }
    pid_t pid = fork();
    if (pid == 0) 
    { 
        if (method == "POST")
        {
            close(input_fd);
            input_fd = open(input_filename.c_str(), O_RDONLY);
            if (input_fd == -1) {
                perror("Failed to reopen input file");
                // exit(1);
                return "500 Internal Server Error\n";
            }
            dup2(input_fd, STDIN_FILENO);
        }
        dup2(output_fd, STDOUT_FILENO);
        close(input_fd);
        close(output_fd);

        std::vector<std::string> env_vars;
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
        std::vector<char*> args;
        args.push_back(const_cast<char*>(interpreter.c_str()));
        args.push_back(const_cast<char*>(path_.c_str()));
        args.push_back(NULL);
        execve(interpreter.c_str(), args.data(), envp.data());
        perror("execve failed");
        //exit(1);
        return "500 Internal Server Error\n";
    }
    else if (pid > 0) 
    {
        close(input_fd);
        close(output_fd);
        waitpid(pid, NULL, 0);
        std::string output;
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        int output_read_fd = open(output_filename.c_str(), O_RDONLY);
        if (output_read_fd == -1) 
        {
            perror("Failed to open output file for reading");
            return "500 Internal Server Error\n";
        }
        while ((bytes_read = read(output_read_fd, buffer, BUFFER_SIZE - 1)) > 0) 
        {
            buffer[bytes_read] = '\0';
            output += buffer;
        }
        close(output_read_fd);
        // unlink(input_filename.c_str());
        // unlink(output_filename.c_str());
        size_t header_end = output.find("\r\n\r\n");
        return (header_end != std::string::npos) ? output.substr(header_end + 4) : output;
    }
    else
    {  
        Message("fork failed", RED);
        return "500 Internal Server Error\n";
    }
}