
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
    std::string root = server.root_location.root;
    std::cout << "Root: " << root << std::endl;
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

    path += (path.back() == '/') ? "index.html" : "";
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
        throw std::runtime_error("Unable to open file of users");
}


void Response::handle_post_request( const std::string &body) {
    std::string uploads = server.upload_location.root;
    std::string root = server.root_location.root;
    std::string post_path = root + "/post-success.html";

    Request request(body);

    if(request.getIsMultipart())
         request.parseMultipartFormData(body);
    else
        return send_error_response(400, "text/html", root + server.error_pages[400]), void();

    set_status(200);
    set_content_type("text/html");
    set_body(read_html_file(post_path));
    send_response();

}



void Response::handle_delete_request(const std::string& body) {
    (void) body;
    std::string root = server.root_location.root;

    std::string uploads = server.upload_location.root;

    std::string delete_path = root + "/delete-success.html";
    std::string delete_error = root + "/delete-failure.html";

    std::string path = uploads;
    if (!std::filesystem::exists(path))
        return send_error_response(404, "text/html", delete_error), void();

    if (remove(path.c_str()) != 0) 
        return send_error_response(500, "text/html", delete_error), void();
    
    set_status(200);
    set_content_type("text/html");
    set_body(read_html_file(delete_path));
    send_response();
}

