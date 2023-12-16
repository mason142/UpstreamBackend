#ifndef CUSTOMSOCKET_H
#define CUSTOMSOCKET_H
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <string>


class CustomSocket {
    public:
        char* ip;
        int port;
        int sd, ret;
        struct sockaddr_in address;
        struct in_addr ipv4addr;
        struct hostent *hp;
        CustomSocket();
        int connectSocket(std::string ip, int port);
        int sendSocket(char *data, int len);
};
#endif