#include "include/CustomSocket.h"
#include <string.h>

CustomSocket::CustomSocket() {

}

int CustomSocket::connectSocket(std::string ip, int port) {
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
    }
 
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
 
    if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        std::cout << "Address was " << ip.c_str() << "\n"; 
        return -1;
    }

    int status = 0;
    if ((status
         = connect(sd, (struct sockaddr*)&address,
                   sizeof(address)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return 1;
}

int CustomSocket::sendSocket(char *data, int len) {
    return send(sd, data, len, 0);
}