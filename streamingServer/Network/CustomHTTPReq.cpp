#include "../include/CustomHTTPReq.h"

CustomHTTPReq::CustomHTTPReq() {
    _verb = std::string("");
    _location = std::string("");
    _version = std::string("");
    _headers = std::unordered_map<std::string, std::string>();
    _type = std::string("");
    _body = std::vector<char>();
}

CustomHTTPReq::~CustomHTTPReq() {
}

void CustomHTTPReq::parseHeader(const std::string& headerLine) {
    size_t pos = headerLine.find(':');
    if (pos != std::string::npos) {
        std::string name = headerLine.substr(0, pos);
        std::string value = headerLine.substr(pos + 2); // Skip ':' and space
        _headers[name] = value;
        if (name == "Content-Type") {
            _type = value;
        }
    }
}

void CustomHTTPReq::parseBody(std::istringstream& iss, int contentLength) {
    const int bufferSize = contentLength;
    char buffer[bufferSize + 1];
    iss.read(buffer, bufferSize);

    if (iss.eof()) {
        return;
    }

    buffer[bufferSize] = '\0';
    std::vector<char> temp(buffer, buffer + bufferSize);

}

bool CustomHTTPReq::parseRequest(const std::string& httpRequest) {

    std::istringstream iss(httpRequest);

    iss >> this->_verb >> _location >> _version;


    if (_verb != std::string("POST") && _verb != std::string("GET")) {
        return false;
    }

    if (_version != std::string("HTTP/1.1")) {
        return false;
    }

    std::string line;
    std::getline(iss, line);
    std::getline(iss, line);
    while (iss && line != std::string("\r")) {
        parseHeader(line);
        std::getline(iss, line);
    }
    
    if (iss.eof()) {
        std::cout << "Reached end of file\n";
        return false;
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

void CustomHTTPReq::setBody(const std::vector<char>& charVector) {
    _body = charVector;
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

