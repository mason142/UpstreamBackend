#include "./streamingServer.h"

void handleConnection(CustomHTTPReq* req, StreamSocket& ss);

int handleBody(CustomHTTPReq *req, StreamSocket& ss, int bytesRec);

int readHTTPHeader(char *bufferBase, StreamSocket &ss);