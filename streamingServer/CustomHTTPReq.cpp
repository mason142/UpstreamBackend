#include "include/CustomHTTPReq.h"

CustomHTTPReq::CustomHTTPReq() {
    _verb = std::string("");
    _location = std::string("");
    _version = std::string("");
    _type = std::string("");
    _length = 0;
    _keepAlive = false;
}

CustomHTTPReq::~CustomHTTPReq() {
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

    std::string header;
    while (std::getline(iss, header) && header != "\r") {
        size_t pos = header.find(':');
        if (pos != std::string::npos) {
            std::string key = header.substr(0, pos);
            std::string value = header.substr(pos + 1);
            value.erase(0, value.find_first_not_of(' '));
            value.erase(value.find_last_not_of(' ') + 1);

            if (key == "Content-Type") {
                _type = value;
            } else if (key == "Content-Length") {
                _length = std::stoi(value);
            } else if (key == "Connection")  {
                if (value == "keep-alive") {
                    _keepAlive = true;
                }
                else {
                    _keepAlive = false;
                }
            } else {
                return false;
            }
        }
    }

    char c;
    while (iss.get(c)) {
        _body.push_back(c);
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

void CustomHTTPReq::setType(const std::string& type) {
    _type = type;
}

void CustomHTTPReq::setLength(int length) {
    _length = length;
}

void CustomHTTPReq::setKeepAlive(bool keepAlive) {
    _keepAlive = keepAlive;
}

void CustomHTTPReq::printReqDetails() {
    std::cout << "Verb: " << _verb << std::endl;
    std::cout << "Location: " << _location << std::endl;
    std::cout << "Version: " << _version << std::endl;
    std::cout << "Content-Type: " << _type << std::endl;
    std::cout << "Content-Length: " << _length << std::endl;

    std::cout << "Body: ";
    for (char c : _body) {
        std::cout << c;
    }
    std::cout << std::endl;
}

std::string CustomHTTPReq::formatHeader() {
    std::ostringstream details;

    details << "Verb: " << _verb << _location << _version << "\n";
    details << "Content-Type: " << _type << "\n";
    details << "Content-Length: " << _length << "\n";

    details << "\r\n";
    for (char c : _body) {
        details << c;
    }
    details << "\n";

    return details.str();
}

