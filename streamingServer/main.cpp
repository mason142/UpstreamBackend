#include "include/streamingServer.h"
#include "include/CustomHTTPReq.h"
#include "include/CustomHTTPRes.h"
#include "include/CustomSocket.h"
#include "include/StreamVideoBuffer.h"
#include <chrono>
#include <thread>

std::map<int, std::vector<StreamVideoBuffer*>> liveStreams;
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
		
		void streamVideo(int streamId, CustomHTTPReq *req) {
			std::cout << "In Stream Video\n";
			if (!isID1Streaming) { return; }
			if (stvbInd1 == 0) { sleep(1000); }
			int currentStreamBufInd = (stvbInd1 - 1) % 10;
			currentStreamBufInd = 0;
			CustomSocket sock = CustomSocket();
			std::string ip = req->getHeaders()["Client-IP"];
			std::string port = req->getHeaders()["Client-Port"];
			std::cout << "Going to send data on client IP = " << ip.c_str() << "Port number = " << port << '\n';
			ip = ip.substr(0, ip.size() - 1);
			port = port.substr(0, port.size() - 1);
			if (sock.connectSocket(ip.c_str(), std::stoi(port)) == -1) {
				std::cout << "Couldnt make sock\n";
				return;
			}
			std::vector<StreamVideoBuffer*> currentStreamBuf = liveStreams[streamId];
			StreamVideoBuffer *streamBuff = currentStreamBuf[currentStreamBufInd];
			int stage = 1;
			int pFramesSent = 0;
			std::cout << "About to send data from stvbInd = " << currentStreamBufInd << '\n';
			while (1) {
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
				int sent = 0;
				if (stage == 1) {
					sent += sock.sendSocket(streamBuff->sps, streamBuff->sizeSPS);
					std::cout << "Sending SPS, size = " << streamBuff->sizeSPS << "\n";
					sent += sock.sendSocket(streamBuff->pps, streamBuff->sizePPS);
					std::cout << "Sending PPS, size = " << streamBuff->sizePPS << "\n";
					sent += sock.sendSocket(streamBuff->preIFrame, streamBuff->sizepreIFrame);
					std::cout << "Sending preIFrame, size = " << streamBuff->sizepreIFrame << "\n";
					sent += sock.sendSocket(streamBuff->iFrame, streamBuff->sizeiFrame);
					std::cout << "Sending iFrame, size = " << streamBuff->sizeiFrame << "\n";
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
							if (streamBuff->hasWriter) {
								return;
							}
						}
					}
				}
			}
		}

		void recordVideo(CustomHTTPReq* req, StreamSocket &ss) {
			std::vector<StreamVideoBuffer*> stream = liveStreams[1]; //hardcoded...
			int stvbInd = 0;
			isID1Streaming = 1;
			StreamVideoBuffer *stvb = stream[stvbInd];
			int stage = 2;
			stvb->addSPS(req->getBody());
			delete req;
			int pFramesRecv = 0;
			while (stvbInd < 10) {
				char buffer[8192];
				//int n = ss.receiveBytes(buffer, sizeof(buffer));
				//std::cout << "About to read header in loop\n";
				int n = readHTTPHeader(buffer, ss);
				CustomHTTPReq *request = new CustomHTTPReq();
				//std::string msg;
				//Logger::formatDump(msg, buffer, n);
				//std::cout << msg << std::endl;
				std::string http(buffer); 
				if (request->parseRequest(http)) {
					//std::cout <<  "Parsed request\n";
					//std::cout << request->getVerb().c_str();
					int type = handleBody(request, ss, n);
					//std::cout << "Type" << type << "\n";
					if (type == 7) {
						pFramesRecv = 0;
						stvb->hasWriter = 0;
						stvbInd++;
						stvbInd1 = stvbInd;
						std::cout << "Recording stvbInd = " << (stvbInd % 10) << "\n";
						if (stvbInd == 10) {
							stvbInd = 1;
						}
						stvb = stream[(stvbInd % 10)];
						assert(stvb);
						stvb->clean();
						std::cout << "Pointer : " << stvb;
						std::cout << "\nSAVING SPS\n\n";
						if (!stvb->addSPS(req->getBody())) {
							std::cout << "\nERROR AT STAGE" << stage << "\n";
						}
					}
					if (type == 8) {
						std::cout << "SAVING PPS\n\n";
						if (!stvb->addPPS(request->getBody())) {
							std::cout << "\nERROR AT STAGE" << stage << "\n";
						}
					}
					if (type == 6) {
						std::cout << "SAVING MYSTERY\n";
						if (!stvb->addpreIFrame(request->getBody())) {
							std::cout << "\nERROR AT STAGE" << stage << "\n";
						}
					}
					else if (type == 5) {
						std::cout << "SAVING IFRAME\n";
						if (!stvb->addIFrame(request->getBody())) {
							std::cout << "\nERROR AT STAGE" << stage << "\n";
						}
					}
					else if (type == 1) {
						//std::cout << "SAVING PFRAME " << pFramesRecv << "\n";
						if (!stvb->addpFrame(request->getBody())) {
							std::cout << "\nERROR AT STAGE" << stage << "\n";
						}
						pFramesRecv++;
					}
				}
				else {
					std::cout << "\n\n\n\n" << "Big time parse error\n\n\n\n";
				}
				delete request;
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

				std::cout << "\n" << std::string(req->getHeaders()["Content-Type"]) << "\n";
				std::cout << (req->getLocation() == std::string("/streams/1")) << " " << (std::string(req->getHeaders()["Content-Type"]) == std::string("UpstreamedVideo\r")) << '\n';
				if (req->getLocation() == std::string("/streams/1") && std::string(req->getHeaders()["Content-Type"]) == std::string("UpstreamedVideo\r")) {
					recordVideo(req, ss);
				}
			}
			if (req->getVerb() == std::string("GET")) {
				if (req->getLocation() == std::string("/streams/1") && std::string(req->getHeaders()["Content-Type"]) == std::string("StreamRequest\r")) {
					streamVideo(1, req);
				}
			}
		}

		int handleBody(CustomHTTPReq *req, StreamSocket& ss, int bytesRec) {
			if (req->getVerb() == std::string("POST")) {
				std::unordered_map<std::string, std::string> headers = req->getHeaders();
				int contentLength = std::stoi(headers["Content-Length"]);
				//std::cout << "Content length is " << contentLength << " req->body " << req->getBody().size() << '\n';
				if (req->getBody().size() != contentLength) {
					char buffer[contentLength];
					//std::cout << sizeof(buffer);
					int n = ss.receiveBytes(buffer, sizeof(buffer) - 1);
					while (n != contentLength) {
						char *temp_buffer = &buffer[n];
						n += ss.receiveBytes(temp_buffer, contentLength - n);
						//std::cout << "This better be equal to content length" << n << "\n";
					}
					//std::cout << "The last 5 bits are :" << (buffer[4] & 0x1F);
					size_t arraySize = sizeof(buffer) / sizeof(char);
					std::vector<char> charVector(buffer, buffer + arraySize);
					req->setBody(charVector);
					//std::string msg;
					//Logger::formatDump(msg, buffer, contentLength);
					//std::cout << msg <<	 std::endl;
					//std::cout << "Body is now" << buffer;
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
			//std::cout << index;
			return index;
		}

		void run()
		{
			std::cout << "New Connection";
			StreamSocket& ss = socket();
			try
			{
				char buffer[8192];
				//int n = ss.receiveBytes(buffer, sizeof(buffer));
				std::cout << "About to read header\n";
				int n = readHTTPHeader(buffer, ss);
				std::cout << n;
				while (n > 0)
				{ 
					CustomHTTPReq *request = new CustomHTTPReq();
					std::string msg;
					Logger::formatDump(msg, buffer, n);
					std::cout << msg << std::endl;
					std::cout << "Received " << n << " bytes:" << std::endl;
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