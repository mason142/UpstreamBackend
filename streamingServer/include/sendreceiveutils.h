#include "./streamingServer.h"

void streamVideo(int streamId, CustomHTTPReq *req); 

void streamSlowVideo(int streamId, CustomHTTPReq *req);

void recordVideo(CustomHTTPReq* req, StreamSocket &ss);

void recordSlowVideo(CustomHTTPReq* req, StreamSocket &ss);