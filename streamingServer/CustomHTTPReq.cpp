#include "include/CustomHTTPReq.h"

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
        //std::cout << "Name = " << name << "Value = " << _headers[name]<< "\n";
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
    //std::cout << buffer << "\n";
    _body = temp;
}

bool CustomHTTPReq::parseRequest(const std::string& httpRequest) {
    
    //std::cout << "In parse request.\n";

    std::istringstream iss(httpRequest);

    iss >> this->_verb >> _location >> _version;


    if (_verb != std::string("POST") && _verb != std::string("GET")) {
        return false;
    }

    if (_version != std::string("HTTP/1.1")) {
        return false;
    }

    //std::cout << this->getVerb().c_str() << "Passed the checks\n";
    
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
    //std::unordered_map<std::string, std::string>::const_iterator got = _headers.find("Content-Length");
    //if (got != _headers.end()) {
    //    int contentLength = std::stoi(got->second);
    //    parseBody(iss, contentLength);
   // }
    //else {
    //    std::cout << "No content-Length found\n";
    //}
    //std::cout << "Size of body : "<< _body.size() << "\n";
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

