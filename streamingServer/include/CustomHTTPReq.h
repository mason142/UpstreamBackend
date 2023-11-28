#include <iostream>
#include <sstream>
#include <vector>

class CustomHTTPReq {
    private:
        std::string _verb;
        std::string _location;
        std::string _version;
        std::string _type;
        int _length;
        bool _keepAlive;
        std::vector<char> _body;

    public:
        CustomHTTPReq();
        ~CustomHTTPReq();

        bool parseRequest(const std::string& httpRequest);

        void setVerb(const std::string& verb);
        void setLocation(const std::string& loc);
        void setVersion(const std::string& ver);
        void setType(const std::string& type);
        void setLength(int length);
        void setKeepAlive(bool ka);

        void printReqDetails();
        std::string formatHeader();

        std::string getVerb() {return _verb; }
        std::string getLocation() {return _location; }
        std::string getVersion() {return _version; }
        std::string getType() {return _type; }
        int getLength() { return _length; }
        bool getKeepAlive() { return _keepAlive; }
};