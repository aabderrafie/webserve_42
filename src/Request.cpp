#include "Request.hpp"

Request::Request(const string &body) {
    std::cout << body << std::endl;
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
    if(line.find("multipart/form-data") != std::string::npos) 
        isMultipart = true;
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




void Request::parseMultipartFormData(const std::string& body) {
    std::string body_copy;
    size_t startPos = body.find("filename=\"");
    size_t endPos = std::string::npos; 
    if (startPos != std::string::npos) {
        startPos += 10; 
        endPos = body.find("\"", startPos);
        if (endPos == std::string::npos) 
            throw std::runtime_error("Filename not found!");
        filename = body.substr(startPos, endPos - startPos);
    } 
        else 
            throw std::runtime_error("Filename not found!");
    body_copy = body.substr(endPos + 2);
    std::map<std::string, std::string> headers = getHeaders();
    std::string boundary = headers["Content-Type"];
    boundary = boundary.substr(boundary.find("boundary=") + 9);
    if(boundary.empty())
        throw std::runtime_error("Boundary not found!");

 std::istringstream iss(body_copy);
 std::string fileContent,line;
std::getline(iss, line);
std::getline(iss, line);
std::getline(iss, line);
while(std::getline(iss, line)){
    fileContent += line;
    fileContent += "\n";
}

size_t boundaryEnd = fileContent.find("WebKitFormBoundary") - 8;
fileContent = fileContent.substr(0, boundaryEnd);
    std::ofstream file(filename, std::ios::binary);
    file.write(fileContent.c_str(), fileContent.size());
    file.close();

}