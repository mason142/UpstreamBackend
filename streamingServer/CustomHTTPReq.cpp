#include "include/CustomHTTPReq.h"

CustomHTTPReq::CustomHTTPReq() {
    _verb = std::string("");
    _location = std::string("");
    _version = std::string("");
    _headers;
}

CustomHTTPReq::~CustomHTTPReq() {
}

void CustomHTTPReq::parseHeader(const std::string& headerLine) {
    // Assuming a simple header format like "He aderName: HeaderValue"
    size_t pos = headerLine.find(':');
    if (pos != std::string::npos) {
        std::string name = headerLine.substr(0, pos);
        std::string value = headerLine.substr(pos + 2); // Skip ':' and space
        _headers[name] = value;
    }
}

void CustomHTTPReq::parseBody(std::istringstream& iss, int contentLength) {
    const int bufferSize = contentLength;
    char buffer[bufferSize];
    iss.read(buffer, bufferSize);
    std::vector<char> _body(buffer, buffer + bufferSize);
}

bool CustomHTTPReq::parseRequest(const std::string& httpRequest) {

    std::istringstream iss(httpRequest);

    iss >> _verb >> _location >> _version;

    if (_verb != "GET" && _verb != "POST") {
        return false;
    }

    if (_location != "/") {
        return false;
    }

    if (_version != "HTTP/1.1") {
        return false;
    }
    std::string line;
    while (std::getline(iss, line) && !line.empty()) {
        parseHeader(line);
    }
    if (_headers.find("Content-Length") != _headers.end()) {
        int contentLength = std::stoi(_headers["Content-Length"]);
        parseBody(iss, contentLength);
    }
    return true;
}

void CustomHTTPReq::setVerb(const std::string& verb) {
    _verb = verb;
}

void CustomHTTPReq::setLocation(const std::string& loc) {
    _location = loc;
}

void CustomHTTPReq::setVersion(const std::string& ver) {
    _version = ver;
}

std::string CustomHTTPReq::formatHeader() {
    std::ostringstream details;

    details << "Verb: " << _verb << _location << _version << "\n";
    // need to add headers..
    details << "\r\n";
    for (char c : _body) {
        details << c;
    }
    details << "\n";

    return details.str();
}

