#include "Request.hpp"

int extractSessionID(const std::string& ref) {
    std::string session_id;
    size_t pos = ref.find("session_id=");
    if (pos != std::string::npos) {
        pos += 11;
        session_id = ref.substr(pos);
        size_t semicolon_pos = session_id.find(";");
        if (semicolon_pos != std::string::npos) {
            session_id.erase(semicolon_pos);
        }
    }
    return std::stoi(session_id);
}

// bool validateSessionID( int _ID ) {
//     std::ifstream file("sessions.txt");
//     if (file.is_open()) {
//         std::string line;
//         std::cout << "loading sessions file.." << std::endl;
//         while (std::getline(file, line)) {
//             size_t start_pos = line.find("session_id=") + std::string("session_id=").length();
//             size_t end_pos = line.find(';');
            
//             if (start_pos != std::string::npos && end_pos != std::string::npos) {
//                 std::string session_id_str = line.substr(start_pos, end_pos - start_pos);
//                 int session_id = std::stoi(session_id_str);
//                 if (_ID == session_id)
//                     return true;
//             } else {
//                 std::cerr << "Invalid session ID format in file: " << line << std::endl;
//             }
//         }
//         file.close();
//     } else {
//         std::cerr << "Unable to open sessions file for reading" << std::endl;
//     }
//     return false;
// }

Request::Request(const string &body)
{
    // std::cout << "------------------------------" << std::endl;
    // std::cout << "Request body: " << body << std::endl;
    // std::cout << "------------------------------" << std::endl;
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
    
    if (body.find("Cookie: ") != std::string::npos) {
        // std::cout << "Cookies found: ";
        size_t cookie_pos = body.find("Cookie: ") + 8;
        size_t cookie_end = body.find("\r\n", cookie_pos);
        cookies = body.substr(cookie_pos, cookie_end - cookie_pos);
        // std::cout << cookies << std::endl;
        // isInNeedOfCookies = false;
        // int _id = extractSessionID(cookies);
        //check for session id after reading file
        // if (!validateSessionID(_id))
        //     throw std::runtime_error("Invalid session");
    }
    //  else {
        // std::cout << "No cookies found, creating new session ID" << std::endl;
        // cookies = "session_id=" + std::to_string(Gid);
        // std::cout << "assigning cookies to: " << cookies << std::endl;
        // this->session_id = Gid;
        // Gid += 10;
        // isInNeedOfCookies = true;
    // }
    
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
        return std::stoi(headers["Content-Length"]);
    return -1;
}
