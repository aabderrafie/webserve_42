#include "mime_types.hpp"

MimeTypes::MimeTypes() {
    mime_types[".html"] = "text/html";
    mime_types[".css"] = "text/css";
    mime_types[".js"] = "application/javascript";
    mime_types[".png"] = "image/png";
    mime_types[".jpg"] = "image/jpeg";
    mime_types[".gif"] = "image/gif";
    mime_types[".swf"] = "application/x-shockwave-flash";
    mime_types[".txt"] = "text/plain";
    mime_types[".mp3"] = "audio/mpeg";
    mime_types[".mp4"] = "video/mp4";
    mime_types[".avi"] = "video/x-msvideo";
    mime_types[".pdf"] = "application/pdf";
    mime_types[".doc"] = "application/msword";
    mime_types[".xls"] = "application/vnd.ms-excel";
    mime_types[".ppt"] = "application/vnd.ms-powerpoint";
    mime_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";

}

MimeTypes::~MimeTypes() {}

std::string MimeTypes::get_mime_type(const std::string &path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string ext = path.substr(dot_pos);
        std::map<std::string, std::string>::const_iterator it = mime_types.find(ext);
        if (it != mime_types.end()) 
            return it->second;
    }
    return "application/octet-stream";
}