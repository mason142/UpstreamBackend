#ifndef CUSTOMHTTPRES_H
#define CUSTOMHTTPRES_H
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>

class CustomHTTPRes {
    private:
        int statusCode;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    public:
        CustomHTTPRes(int statusCode);
        void addHeader(const std::string& name, const std::string& value);
        void setBody(const std::string& bodyContent);
        std::string toString();
        std::string getStatusMessage();
};
#endif