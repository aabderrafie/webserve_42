#include "tokenizer.hpp"

Tokenizer::Tokenizer() {}

Tokenizer::~Tokenizer() {}

void Tokenizer::tokenize(const std::string& configContent) {
    std::vector<std::string> lines;
    lines = split(configContent, '\n');
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        if ((*it)[0] == '#' || (*it).empty()) {
            lines.erase(it);
            it--;
        }
    }
    //print lines
    // for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) //debug
    //     std::cout << *it << std::endl;   //debug
    std::vector<std::string> splited_words;
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        std::vector<std::string> tmp;
        tmp = split(*it, ' ');
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
            splited_words.push_back(*it);
        }
    }
    std::vector<std::string> final_tokens;
    for (std::vector<std::string>::iterator it = splited_words.begin(); it != splited_words.end(); ++it) {
        std::vector<std::string> tmp;
        tmp = split(*it, ';');
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
            final_tokens.push_back(*it);
        }
        if ((*it).find(';') != std::string::npos) {
            final_tokens.push_back(";");
        }
    }
    std::string delayed;
    std::string checker;
    for (std::vector<std::string>::iterator it = final_tokens.begin(); it != final_tokens.end(); ++it) {
        Token token;
        token.value = *it;
        if (*it == "{" || *it == "}" || *it == ";") {
            token.type = "symbol";
        } else if (*it == "server" || *it == "location") {
            token.type = "block";
        } else if (delayed == "symbol" && (*it == "listen"
                || *it == "host"
                || *it == "port" 
                || *it == "server_name"
                || *it == "is_default_server" 
                || *it == "client_max_body_size" 
                || *it == "error_page"
                || *it == "allowed_methods"
                || *it == "root_directory" 
                || *it == "directory_listing" 
                || *it == "default_file" 
                || *it == "upload_path"
                || *it == "cgi"
                || *it == "redirect")) {
            token.type = "directive";
        } else
            token.type = "value";
        if (delayed == "symbol" && token.type == "value") throw std::runtime_error("Error: unexpected value");
        if (delayed == "value" && token.type == "directive") throw std::runtime_error("Error: missign semicolon");
        if (delayed == "directive" && token.value == ";") throw std::runtime_error("Error: missign value");
        tokens.push_back(token);
        delayed = token.type;
    }
}