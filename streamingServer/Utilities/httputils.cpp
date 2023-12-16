#include "../include/streamingServer.h"

int handleBody(CustomHTTPReq *req, StreamSocket& ss, int bytesRec) {
    if (req->getVerb() == std::string("POST")) {
        std::unordered_map<std::string, std::string> headers = req->getHeaders();
        int contentLength = std::stoi(headers["Content-Length"]);
        if (req->getBody().size() != contentLength) {
            char buffer[contentLength];
            int n = ss.receiveBytes(buffer, sizeof(buffer) - 1);
            while (n != contentLength) {
                char *temp_buffer = &buffer[n];
                n += ss.receiveBytes(temp_buffer, contentLength - n);
            }
            size_t arraySize = sizeof(buffer) / sizeof(char);
            std::vector<char> charVector(buffer, buffer + arraySize);
            req->setBody(charVector);
            return buffer[4] & 0x1F;
        }
    }
    return 0;
}

int readHTTPHeader(char *bufferBase, StreamSocket &ss) {
    char *buffer = bufferBase;
    int index = 0;
    while (index < 4 || (bufferBase[index-1]!='\n'||bufferBase[index-2]!='\r'||bufferBase[index-3]!='\n'||bufferBase[index-4]!='\r')) {
        int n = ss.receiveBytes(buffer, 1);
        if (n > 0) {
            buffer++;
            index++;
            if (index > 2048) {
                std::cout << "Buffer overflow!\n";
                std::cout << bufferBase;
                return -1;
            }
        }
    }
    return index;
}