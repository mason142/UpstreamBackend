#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>

class CustomHTTPReq {
    private:
        std::string _verb;
        std::string _location;
        std::string _version;
        std::vector<char> _body;
        std::unordered_map<std::string, std::string> _headers;

    public:

        CustomHTTPReq();
        ~CustomHTTPReq();

        void parseHeader(const std::string& headerLine);
        bool parseRequest(const std::string& httpRequest);
        void parseBody(std::istringstream& iss, int contentLength);

        void setVerb(const std::string& verb);
        void setLocation(const std::string& loc);
        void setVersion(const std::string& ver);
        void setBody(const char* buffer);
        std::string formatHeader();

        std::string getVerb() {return this->_verb; }
        std::string getLocation() {return _location; }
        std::string getVersion() {return _version; }
        std::unordered_map<std::string, std::string> getHeaders() {return _headers; }
        std::vector<char> getBody() { return _body; }
};