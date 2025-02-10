
#include "Response.hpp"

Response::Response(int client_socket, const Server& server) : client_socket(client_socket), mime_types(),server(server) {}

Response::~Response() {}

void Response::set_content_type(const std::string &content_type) {this->content_type = content_type;}

void Response::set_status(int status) {this->status = status;}

void Response::set_body(const std::string &body) {this->body = body;}

bool Response::is_valid_url(const std::string& url) {
    const std::string valid_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "@:%.+~#-_/=?";
    for (size_t i = 0; i < url.length(); ++i) {
        char c = url[i];
        if (valid_chars.find(c) == std::string::npos)
            return false;
    }
    return true;
}

bool Response::is_directory(const std::string &url) {
    struct stat file_stat;
    if (stat(url.c_str(), &file_stat) != 0)
        return false;
    return S_ISDIR(file_stat.st_mode);
}

std::string Response::read_html_file(const std::string& file_path) {
    std::ifstream file(file_path.c_str());
    if (!file.is_open()) 
        throw std::runtime_error("Unable to open file: " + file_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Response::send_error_response(int status, const std::string& content_type, const std::string& error_page_path) {
    set_status(status);
    set_content_type(content_type);
    set_body(read_html_file(error_page_path));
    send_response();
}

void Response::send_response() {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n"
            //  << "Set-Cookie: " << Cookies[0] << "\r\n"
            //  << "Set-Cookie: " << Cookies[1] << "\r\n"
    // for (std::vector<std::string>::const_iterator it = request.cookies.begin(); it != request.cookies.end(); ++it)
    //     response << "Set-Cookie: " << *it << "\r\n";
    // }
    // response << "Content-Type: " << content_type << "\r\n"
            << "Content-Type: " << content_type << "\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n";
    std::string headers = response.str();
    send(client_socket, headers.c_str(), headers.size(), 0);

    const size_t chunk_size = 1024; 
    size_t sent = 0;
    while (sent < body.size()) {
        size_t to_send = std::min(chunk_size, body.size() - sent);
        send(client_socket, body.c_str() + sent, to_send, 0);
        sent += to_send;
    }
}

void Response::handle_get_request(const std::string &body) {
    (void) body;
    std::string root = server.locations["/"].root;
    std::string uri = request.getPath();
    
    if (!is_valid_url(uri))
       return  send_error_response(400, "text/html", server.error_pages[400]), void();

    if (uri.find("..") != std::string::npos)
        return send_error_response(403, "text/html", server.error_pages[403]), void();
    
    if (uri.length() > 2048)
        return send_error_response(414, "text/html", server.error_pages[414]), void();

    std::string path = root + uri;
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open() ) 
       return send_error_response(404, "text/html", server.error_pages[404]), void();

    path += (path.back() == '/') ? server.locations["/"].default_file : "";
    set_status(200);
    set_content_type(mime_types.get_mime_type(path));
    set_body(read_html_file(path));
    send_response();
 }



std::map<std::string, std::string> parse_post_data(const std::string& body) {
    std::map<std::string, std::string> data;
    std::istringstream stream(body);
    std::string line;

    std::getline(stream, line);
    while (std::getline(stream, line)) {
        if (line.find(": ") == std::string::npos ||   line[0] == '\n') {
            std::istringstream result(line);
            std::string temp;
            while (std::getline(result, temp, '&')) 
                data.insert( std::make_pair(temp.substr(0, temp.find('=')), temp.substr(temp.find('=') + 1)));
        }
    }
    if(!data.empty())
        data.erase(data.begin());
    return data;
}





void Response::create_user(const std::map<std::string, std::string>& data, const std::string& uploads) {
    auto username_it = data.find("username");
    auto password_it = data.find("password");
    auto fullname_it = data.find("fullname");
    if (username_it == data.end() || password_it == data.end() || fullname_it == data.end())
        throw std::runtime_error("Missing required user data");

    std::string username = username_it->second;
    std::string password = password_it->second;
    std::string fullname = fullname_it->second;

    std::ofstream user_file(uploads + "/" + username + ".txt");
    if (user_file.is_open()) {
        user_file << "User Information" << std::endl;
        user_file << "-----------------" << std::endl;
        user_file << "Username: " << username << std::endl;
        user_file << "Password: " << password << std::endl;
        user_file << "Fullname: " << fullname << std::endl;
        user_file.close();
    } 
    else 
       std::cerr << RED << "Unable to create user file" << RESET << std::endl;
}

void Response::upload_file(std::string& uploaded_file_path) 
{
    std::string post_data = request.getPostData();//body>>
    std::string boundary = request.getBoundary();

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

            uploaded_file_path = "./files/uploads/" + filename;
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

// int getSessionID(const std::string& ref) {
//     std::string session_id;
//     size_t pos = ref.find("session_id=");
//     if (pos != std::string::npos) {
//         pos += 11;
//         while (ref[pos] != ';') {
//             session_id += ref[pos];
//             ++pos;
//         }
//     }
//     return std::stoi(session_id);
// }

// std::map<int, Session> sessions;

void Response::handle_post_request(const std::string &body) {
    (void) body;
    std::string uploads = server.locations["/upload"].root;
    std::string root = server.locations["/"].root;
    std::string post_path = root + request.getPath();
    // server.load_sessions_from_file();

    if (!is_valid_url(request.getPath()))
        return send_error_response(400, "text/html", server.error_pages[400]), void();

    if (request.getPath().find("..") != std::string::npos)
        return send_error_response(403, "text/html", server.error_pages[403]), void();

    if (request.getPath().length() > 2048)
        return send_error_response(414, "text/html", server.error_pages[414]), void();

    if (request.getIsMultipart())
        upload_file(uploads);
    // else if (request.toggleTheme(request.getPostData()) == 2) {
    //     post_path = post_path + "dark" + server.locations["/"].default_file;
    //     server.sessions[getSessionID(request.getCookies()[0])].isDarkMode = 1;
    // } else if (request.toggleTheme(request.getPostData()) == 1) {
    //     post_path = post_path + server.locations["/"].default_file;
    //     server.sessions[getSessionID(request.getCookies()[0])].isDarkMode = 0;
    // }
     else
        return send_error_response(400, "text/html", server.error_pages[400]), void();

    // for (std::map<int, Session>::value_type& session : server.sessions) {
    //     std::cout << "Session ID: " << session.first << std::endl;
    //     std::cout << "Dark Mode: " << session.second.isDarkMode << std::endl;
    // }
    // server.save_sessions_to_file();
    set_status(200);
    set_content_type("text/html");
    set_body(read_html_file(post_path));
    send_response();
}
void Response::handle_delete_request(const std::string& body) {
    (void) body;
    std::string root = server.locations["/"].root;
    std::string uploads = server.locations["/upload"].root + request.getPath();
    std::string delete_path = root + "/delete-success.html";
    std::string delete_error = root + "/delete-failure.html";

    if (!is_valid_url(request.getPath()))
        return send_error_response(400, "text/html", server.error_pages[400]), void();

    if (request.getPath().find("..") != std::string::npos)
        return send_error_response(403, "text/html", server.error_pages[403]), void();

    if (request.getPath().length() > 2048)
        return send_error_response(414, "text/html", server.error_pages[414]), void();

    if (!std::filesystem::exists(uploads))
        return send_error_response(404, "text/html", delete_error), void();

    if (remove(uploads.c_str()) != 0)
        return send_error_response(500, "text/html", delete_error), void();
    set_status(200);
    set_content_type("text/html");
    set_body(read_html_file(delete_path));
    send_response();
}