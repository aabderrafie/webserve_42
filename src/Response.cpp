
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

void Response::list_directory_contents(const std::string& dir_path) {
    DIR* dir;
    struct dirent* ent;
    std::string html_template;
    std::ifstream template_file("./files/html/directory.html");

    if (template_file.is_open()) {
        std::stringstream buffer;
        buffer << template_file.rdbuf();
        html_template = buffer.str();
        template_file.close();
    } 
    else 
        return send_error_response(500, "text/html", server.error_pages[500]) , void();

    std::string directory_items;
    if ((dir = opendir(dir_path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string name = ent->d_name;
    
            if (name != "." && name != "..") {
                directory_items += "<li><a href='";
                directory_items += name;
                directory_items += "'>";
                directory_items += name;
                directory_items += "</a></li>";
            }
        }
        closedir(dir); 
    } else 
        return send_error_response(500, "text/html", server.error_pages[500]) , void();

    size_t pos;
    while ((pos = html_template.find("{DIRECTORY_PATH}")) != std::string::npos) 
        html_template.replace(pos, 15, dir_path);

    while ((pos = html_template.find("{DIRECTORY_ITEMS}")) != std::string::npos)
        html_template.replace(pos, 16, directory_items);

    set_status(200);
    set_content_type("text/html");
    set_body(html_template);
    send_response();
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

std::string Response::get_status_message(int status) {
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 500: return "Internal Server Error";
        default: return "Unknown";
    }
}

void Response::send_response() {
    std::ostringstream response;
   response << "HTTP/1.1 " << status << " " << get_status_message(status) << "\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    std::string headers = response.str();
    std::string full_response = headers + body;
    if(send(client_socket, full_response.c_str(), full_response.size(), 0) < 0)
        throw std::runtime_error("Failed to send response");
}

bool Response::check_error(const std::string& path) {
    std::string uri = request.getPath();
    if (request.getContentLength() > server.client_max_body_size )
        return send_error_response(413, "text/html", server.error_pages[413]) , Message("Payload too large", RED), false;

    if (!is_valid_url(uri))
       return  send_error_response(400, "text/html", server.error_pages[400]) , Message("Invalid URL", RED), false;


    if (uri.find("..") != std::string::npos)
        return send_error_response(403, "text/html", server.error_pages[403]) ,  Message("Forbidden", RED), false;


    if (uri.length() > 2048)
        return send_error_response(414, "text/html", server.error_pages[414]) , Message("URI too long", RED), false;

    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open() ) 
       return send_error_response(404, "text/html", server.error_pages[404]) , Message("File not found", RED), false;

    else    
        return true;
}

void Response::handle_get_request(const std::string &uri) {
    set_status(200);
    set_content_type(mime_types.get_mime_type(uri));
    set_body(read_html_file(uri));
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



void Response::upload_file(std::string& uploaded_file_path) 
{
    std::string post_data = request.getPostData();
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

            uploaded_file_path += filename;
            std::ofstream out_file(uploaded_file_path.c_str(), std::ios::binary);
            if (out_file) {
                out_file.write(file_content.c_str(), file_content.size());
                Message("File uploaded successfully", GREEN);
                out_file.close();
            } else 
                return send_error_response(500, "text/html", server.error_pages[500]), throw std::runtime_error("Failed to upload file") , void();
        }
    }
}

bool file_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

void Response::handle_post_request(const std::string &body) {
    std::string uploads = server.locations["/upload"].root;
    if(!uploads.empty() && uploads[uploads.size() - 1] != '/')
        uploads += '/';
    uploads += "upload/";
    if (request.getIsMultipart() ){
        if(!server.locations["/upload"].allow_upload)
            return  send_error_response(403, "text/html", server.error_pages[403]),Message("Upload not allowed", RED), void();
        upload_file(uploads);
    }
    set_status(201);
    set_content_type("text/html");
    set_body(read_html_file(body));
    send_response();
}
void Response::handle_delete_request() {
    std::string query_string = request.getQueryString();
    query_string = query_string.find_first_of('=') ? query_string.substr(0, query_string.find_first_of('=')) : query_string;
    std::string root = server.locations["/"].root;

    std::string uploads = server.locations["/upload"].root;
    if (!uploads.empty() && uploads[uploads.size() - 1] != '/')
        uploads += '/';
    uploads += "upload" + request.getPath() + query_string;
    std::string delete_path = root + "/delete-success.html";
    std::string delete_error = root + "/delete-failure.html";

    if (!file_exists(uploads)) 
        return send_error_response(404, "text/html", delete_error), Message("File not found", RED), void();
    if (std::remove(uploads.c_str()) != 0)
        return send_error_response(500, "text/html", delete_error), Message("Error deleting file", RED), void();
  
    set_status(200);
    set_content_type("text/html");
    set_body(read_html_file(delete_path));
    send_response();
}