#include "Request.hpp"

Request::Request(const string &body) 
{
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


    //////////////

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
    std::cout << "Request Line: " << line << std::endl;
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




// void Request::parseMultipartFormData(const std::string& body) {
//     std::string body_copy;
//     size_t startPos = body.find("filename=\"");
//     size_t endPos = std::string::npos; 
//     if (startPos != std::string::npos) {
//         startPos += 10; 
//         endPos = body.find("\"", startPos);
//         if (endPos == std::string::npos) 
//             throw std::runtime_error("Filename not found!");
//         filename = body.substr(startPos, endPos - startPos);
//     } 
//         else 
//             throw std::runtime_error("Filename not found!");
//     body_copy = body.substr(endPos + 2);
//     std::map<std::string, std::string> headers = getHeaders();
//     std::string boundary = headers["Content-Type"];
//     boundary = boundary.substr(boundary.find("boundary=") + 9);
//     if(boundary.empty())
//         throw std::runtime_error("Boundary not found!");

//  std::istringstream iss(body_copy);
//  std::string fileContent,line;
// std::getline(iss, line);
// std::getline(iss, line);
// std::getline(iss, line);
// while(std::getline(iss, line)){
//     fileContent += line;
//     fileContent += "\n";
// }

// size_t boundaryEnd = fileContent.find("WebKitFormBoundary") - 8;
// fileContent = fileContent.substr(0, boundaryEnd);
//     std::ofstream file(filename, std::ios::binary);
//     file.write(fileContent.c_str(), fileContent.size());
//     file.close();

// }