#include "include/streamingServer.h"
#include "include/CustomHTTPReq.h"
#include "include/CustomHTTPRes.h"
#include "include/CustomSocket.h"
#include "include/StreamVideoBuffer.h"
#include "include/SlowStreamCircularBuffer.h"
#include <chrono>
#include <thread>

std::map<int, std::vector<StreamVideoBuffer*>> liveStreams;
std::map<int, SlowStreamCircularBuffer*> slowLiveStreams;
int id = 0;
int stvbInd1 = 1;
int isID1Streaming = 0;

namespace
{
	class ClientConnection: public TCPServerConnection
	{
	public:
		ClientConnection(const StreamSocket& s): TCPServerConnection(s)
		{
		}
		
		int createNewLivestreamingSession(std::vector<char> title) {
			std::vector<StreamVideoBuffer*> *stream = new std::vector<StreamVideoBuffer*>;
			for (int i = 0; i < 10; i++) {
				StreamVideoBuffer *s = new StreamVideoBuffer;
				stream->push_back(s);
			}
			liveStreams[++id] = *stream;
			return id;
		}

		int createNewSlowLiveStreamingSession(std::vector<char> title) {
			SlowStreamCircularBuffer *slowStream = new SlowStreamCircularBuffer(60);
			slowStream->allocateFrames();
			slowLiveStreams[++id] = slowStream;
			return id;
		}

		
		void streamVideo(int streamId, CustomHTTPReq *req) {
			if (!isID1Streaming) { return; }
			if (stvbInd1 == 0) { sleep(1); }
			int currentStreamBufInd = (stvbInd1 - 1) % 10;
			currentStreamBufInd = 0;
			std::vector<StreamVideoBuffer*> currentStreamBuf = liveStreams[streamId];
			StreamVideoBuffer *streamBuff = currentStreamBuf[currentStreamBufInd];

			if (!streamBuff) {
				printf("Stream Doesnt Exist\n");
			}
			CustomSocket sock = CustomSocket();
			std::string ip = req->getHeaders()["Client-IP"];
			std::string port = req->getHeaders()["Client-Port"];
			ip = ip.substr(0, ip.size() - 1);
			port = port.substr(0, port.size() - 1);
			if (sock.connectSocket(ip.c_str(), std::stoi(port)) == -1) {
				std::cout << "Couldnt make sock\n";
				return;
			}

			int stage = 1;
			int pFramesSent = 0;
			std::cout << "About to send data from stvbInd = " << currentStreamBufInd << '\n';
			while (1) {
				std::this_thread::sleep_for(std::chrono::milliseconds(33));
				int sent = 0;
				if (stage == 1) {
					sent += sock.sendSocket(streamBuff->sps, streamBuff->sizeSPS);
					std::this_thread::sleep_for(std::chrono::milliseconds(33));
					sent += sock.sendSocket(streamBuff->pps, streamBuff->sizePPS);
					std::this_thread::sleep_for(std::chrono::milliseconds(33));
					sent += sock.sendSocket(streamBuff->preIFrame, streamBuff->sizepreIFrame);
					std::this_thread::sleep_for(std::chrono::milliseconds(33));
					sent += sock.sendSocket(streamBuff->iFrame, streamBuff->sizeiFrame);
					std::this_thread::sleep_for(std::chrono::milliseconds(33));
					if (sent != (streamBuff->sizeiFrame + streamBuff->sizepreIFrame + streamBuff->sizePPS + streamBuff->sizeSPS)) {
						std::cout << "Failed to send all data!\n\n";
					}
					stage = 5;
				}
				if (stage == 5) {
					sent = sock.sendSocket(streamBuff->pFrame[pFramesSent], streamBuff->sizepFrame[pFramesSent]);
					std::cout << "Sending pFrame " << pFramesSent << " size = " << streamBuff->sizepFrame[pFramesSent] << "\n";
					if (sent != streamBuff->sizepFrame[pFramesSent]) {
						std::cout << "Failed to send all pFrame data\n" << "Sent : " << sent << " Expected : " << streamBuff->sizepFrame[pFramesSent] << "\n";
					}
					pFramesSent++;
					if (pFramesSent >= streamBuff->numPFrames) {
						stage = 1;
						if (currentStreamBufInd == 10) {
							currentStreamBufInd = 0;
						}
						streamBuff = currentStreamBuf[(++currentStreamBufInd) % 10];
						std::cout << "About to send data from stvbInd = " << currentStreamBufInd << '\n';
						pFramesSent = 0;
						if (streamBuff->hasWriter) {
							std::cout << "Caught up to streamer, need to sleep\n\n";
							sleep(1);
						}
					}
				}
			}
		}

		void streamSlowVideo(int streamId, CustomHTTPReq *req) {
			std::cout << "In Stream Slow Video\n";
			if (!isID1Streaming) { return; }
			CustomSocket sock = CustomSocket();
			std::string ip = req->getHeaders()["Client-IP"];
			std::string port = req->getHeaders()["Client-Port"];
			//std::cout << "Going to send data on client IP = " << ip.c_str() << "Port number = " << port << '\n';
			ip = ip.substr(0, ip.size() - 1);
			port = port.substr(0, port.size() - 1);
			if (sock.connectSocket(ip.c_str(), std::stoi(port)) == -1) {
				std::cout << "Couldnt make sock\n";
				return;
			}
			SlowStreamCircularBuffer *sscb = slowLiveStreams[streamId];
			if (!sscb) {
				return;
			}
			// Start sending ten frames behind the streamer
			int currentFrame = sscb->currentWriterFrame - 1;
			printf("Current frame = %d\n", currentFrame);
			while (1) {
				while (currentFrame == sscb->currentWriterFrame) {
					sleep(1);
				}
				sock.sendSocket(sscb->frameData[currentFrame], sscb->frameSize[currentFrame]);
				currentFrame++;
			}
		}	

		void recordVideo(CustomHTTPReq* req, StreamSocket &ss) {
			std::vector<StreamVideoBuffer*> stream = liveStreams[1]; //hardcoded...
			int stvbInd = 0;
			isID1Streaming = 1;
			StreamVideoBuffer *stvb = stream[stvbInd];
			if (!stvb) {
				return;
			}
			int stage = 2;
			stvb->addSPS(req->getBody());
			delete req;
			int pFramesRecv = 0;
			while (stvbInd < 10) {
				char buffer[8192];
				int n = readHTTPHeader(buffer, ss);
				CustomHTTPReq *request = new CustomHTTPReq();
				std::string http(buffer); 
				if (request->parseRequest(http)) {
					int type = handleBody(request, ss, n);
					type = std::stoi(request->getHeaders()["NALU-Type"]);
					if (type == 7) {
						pFramesRecv = 0;
						stvb->hasWriter = 0;
						stvbInd++;
						stvbInd1 = stvbInd;
						std::cout << "Recording stvbInd = " << (stvbInd % 10) << "\n";
						stvb = stream[(stvbInd % 10)];
						assert(stvb);
						stvb->clean();
						if (!stvb->addSPS(req->getBody())) {
							std::cout << "\nERROR AT STAGE" << type << "\n";
							delete request;
							return;
						}
					}
					if (type == 8) {
						//std::cout << "SAVING PPS\n\n";
						if (!stvb->addPPS(request->getBody())) {
							delete request;
							return;
							std::cout << "\nERROR AT STAGE" << type << "\n";
						}
					}
					if (type == 6) {
						//std::cout << "SAVING MYSTERY\n";
						if (!stvb->addpreIFrame(request->getBody())) {
							std::cout << "\nERROR AT STAGE" << type << "\n";
							delete request;
							return;
						}
					}
					else if (type == 5) {
						//std::cout << "SAVING IFRAME\n";
						if (!stvb->addIFrame(request->getBody())) {
							std::cout << "\nERROR AT STAGE" << type << "\n";
							delete request;
							return;
						}
					}
					else if (type == 1) {
						//std::cout << "SAVING PFRAME " << pFramesRecv << "\n";
						if (!stvb->addpFrame(request->getBody())) {
							std::cout << "\nERROR AT STAGE" << type << "\n";
							delete request;
							return;
						}
						pFramesRecv++;
					}
				}
				else {
					std::cout << "\n\n\n\n" << "Big time parse error\n\n\n\n";
					delete request;
					return;
				}
				delete request;
			}
		}


		/* For uncompressed video (doesn't work on any real networks)*/
		void recordSlowVideo(CustomHTTPReq* req, StreamSocket &ss) {
			isID1Streaming = 1;
			SlowStreamCircularBuffer *sscb = slowLiveStreams[1];
			sscb->appendFrame(req->getBody());
			delete req;
			while (1) {
				char buffer[8192];
				int n = readHTTPHeader(buffer, ss);
				CustomHTTPReq *request = new CustomHTTPReq();
				std::string http(buffer); 
				if (request->parseRequest(http)) {
					handleBody(request, ss, n);
					sscb->appendFrame(request->getBody());
				}
			}
		}

		void handleConnection(CustomHTTPReq* req, StreamSocket& ss) {
			std::cout << "In handle connection\n";
			if (req->getVerb() == std::string("POST")) {
				if (req->getLocation() == std::string("/streams") && std::string(req->getHeaders()["Data-Type"]) == std::string("Text\r")) {
					std::cout << "About to craft response\n";
					int id = createNewLivestreamingSession(req->getBody());
					CustomHTTPRes response(200);
					response.addHeader("Content-Type", "text/plain");
					std::vector<char> readBuffer = req->getBody();
					std::string body = std::string(readBuffer.begin(), readBuffer.end());
					response.addHeader("Content-Length", std::to_string(1));
					response.setBody(std::to_string(id));

					// Send the HTTP response
					std::string resStr = response.toString();
					int resLen = resStr.length();
					std::cout << resStr;
					int sent = ss.sendBytes(resStr.c_str(), resLen);
					if (sent != resLen) {	
						std::cout << "Sent : " << sent << "Bytes" << "Buffer had :" << resLen << "Bytes \n";
					}
					return;
				}
				if (req->getLocation() == std::string("/slowstreams") && std::string(req->getHeaders()["Data-Type"]) == std::string("Text\r")) {
					std::cout << "Requested Slow Stream\n";
					int id = createNewSlowLiveStreamingSession(req->getBody());
					CustomHTTPRes response(200);
					response.addHeader("Content-Type", "text/plain");
					std::vector<char> readBuffer = req->getBody();
					std::string body = std::string(readBuffer.begin(), readBuffer.end());
					response.addHeader("Content-Length", std::to_string(1));
					response.setBody(std::to_string(id));

					// Send the HTTP response
					std::string resStr = response.toString();
					int resLen = resStr.length();
					std::cout << resStr;
					int sent = ss.sendBytes(resStr.c_str(), resLen);
					if (sent != resLen) {	
						std::cout << "Sent : " << sent << "Bytes" << "Buffer had :" << resLen << "Bytes \n";
					}
					return;
				}
				//hardcoded for stream 1
				if (req->getLocation() == std::string("/streams/1") && std::string(req->getHeaders()["Content-Type"]) == std::string("UpstreamedVideo\r")) {
					recordVideo(req, ss);
				}
				//hardcoded for stream 1
				if (req->getLocation() == std::string("/slowstreams/1") && std::string(req->getHeaders()["Content-Type"]) == std::string("UpstreamedVideo\r")) {
					std::cout << "slowStreams\n";
					recordSlowVideo(req, ss);
				}
			}
			if (req->getVerb() == std::string("GET")) {
				if (req->getLocation() == std::string("/streams/1") && std::string(req->getHeaders()["Content-Type"]) == std::string("StreamRequest\r")) {
					streamVideo(1, req);
				}
				if (req->getLocation() == std::string("/slowstreams/1") && std::string(req->getHeaders()["Content-Type"]) == std::string("StreamRequest\r")) {
					streamSlowVideo(1, req);
				}
			}
		}

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

		void run()
		{
			std::cout << "New Connection";
			StreamSocket& ss = socket();
			try
			{
				char buffer[8192];
				int n = readHTTPHeader(buffer, ss);
				while (n > 0)
				{ 
					CustomHTTPReq *request = new CustomHTTPReq();
					std::string http(buffer); 
					if (request->parseRequest(http)) {
						//std::cout <<  "Parsed request\n";
						std::cout << request->getVerb().c_str();
						handleBody(request, ss, n);
						std::cout << "\nHandled error\n";
						handleConnection(request, ss);
						std::cout << "Im back\n";
					}
					else {

					}
					//request.printReqDetails();
					if (request->getLocation() == std::string("/streams") && request->getVerb() == std::string("POST")) {
						ss.close();
					}
					if (request->getVerb() == std::string("GET")) {
						ss.close();
					}
					delete request;
					n = readHTTPHeader(buffer, ss);
				}
			}
			catch (Exception& exc)
			{
				std::cerr << "ClientConnection: " << exc.displayText() << std::endl;
			}
		}
	};

	typedef TCPServerConnectionFactoryImpl<ClientConnection> TCPFactory;
#if defined(POCO_OS_FAMILY_WINDOWS)
	NamedEvent terminator(ProcessImpl::terminationEventName(Process::id()));
#else
	Event terminator;
#endif
}


int main(int argc, char** argv)
{
	try
	{
		Poco::UInt16 port = NumberParser::parse((argc > 1) ? argv[1] : "9090");
		TCPServer srv(new TCPFactory(), port);
		srv.start();

		std::cout << "TCP server listening on port " << port << '.'
			 << std::endl << "Press Ctrl-C to quit." << std::endl;

		terminator.wait();
	}
	catch (Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
		return 1;
	}

	return 0;
}