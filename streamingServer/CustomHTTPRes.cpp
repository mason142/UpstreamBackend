#include "include/CustomHTTPRes.h"


CustomHTTPRes::CustomHTTPRes(int statusCode) : statusCode(statusCode) {}

void CustomHTTPRes::addHeader(const std::string& name, const std::string& value) {
    headers[name] = value;
}

void CustomHTTPRes::setBody(const std::string& bodyContent) {
    body = bodyContent;
}

std::string CustomHTTPRes::toString() {
    std::ostringstream os;
    os << "HTTP/1.1 " << statusCode << " " << CustomHTTPRes::getStatusMessage() << "\r\n";

    for (const auto& header : headers) {
        os << header.first << ": " << header.second << "\r\n";
    }

    os << "\r\n"; // End of headers
    os << body <<  "\r\n" << std::endl;
    return os.str();
}

std::string CustomHTTPRes::getStatusMessage(){
    switch (statusCode) {
        case 200: return "OK";
        case 404: return "Not Found";
        default: return "Unknown";
    }
}

