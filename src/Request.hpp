#pragma once
#include "../includes/include.hpp"
using namespace std;

class Request {
    private:
        string method;
        string path;
        string httpVersion;
        map<string, string> headers;
        map<string, string> formData;
        bool isMultipart = false;
        bool isUrlEncoded = false;
        string boundary;
        string filename;
        string fileContent;

    public:
        Request(const string &body);
        void parseRequestLine(const string& line);
        void parseHeader(const string& line);
        void parseUrlEncodedData(const string& body);
        void parseMultipartFormData(const string& body);
        string trim(string str);


        const string& getMethod() const { return method; }
        const string& getPath() const { return path; }
        const string& getHttpVersion() const { return httpVersion; }
        const map<string, string>& getHeaders() const { return headers; }
        const map<string, string>& getFormData() const { return formData; }
        const bool& getIsMultipart() const { return isMultipart; }
        const string& getFilename() const { return filename; }
        const string& getFileContent() const { return fileContent; }
        const bool& getIsUrlEncoded() const { return isUrlEncoded; }


};