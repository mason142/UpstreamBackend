#include "include/streamingServer.h"
#include "include/CustomHTTPReq.h"
#include "include/CustomHTTPRes.h"


namespace
{
	class ClientConnection: public TCPServerConnection
	{
	public:
		ClientConnection(const StreamSocket& s): TCPServerConnection(s)
		{
		}
/*
		std::string findInMap(std::unordered_map<std::string, std::string> p, std::string lookFor) {
			std::unordered_map<std::string, std::string>::iterator it;
			for (it = p.begin(); it != p.end(); it++) {
				std::cout << it->first << ": "<< it->second.c_str() << "\n";
				if (std::string(it->first.c_str()) == lookFor) {
					std::cout << it->second.c_str();
					return std::string(it->second.c_str);
				}
			}
			return std::string("You need to handle this");
		}
		*/

		void handleConnection(CustomHTTPReq* req, StreamSocket& ss) {
			std::cout << "In handle connection\n";
			if (req->getVerb() == std::string("POST")) {
				if (req->getLocation() == std::string("/streams") && std::string(req->getHeaders()["Data-Type"]) == std::string("Text\r")) {
					std::cout << "About to craft response\n";
					// Allocate memory for a new stream 
					// Assuming that the memory allocation worked
					CustomHTTPRes response(200);
					response.addHeader("Content-Type", "text/plain");
					std::vector<char> readBuffer = req->getBody();
					std::string body = std::string(readBuffer.begin(), readBuffer.end());
					response.addHeader("Content-Length", std::to_string(body.size()));
					response.setBody(body.c_str());

					// Send the HTTP response
					std::string resStr = response.toString();
					int resLen = resStr.length();
					std::cout << resStr;
					int sent = ss.sendBytes(resStr.c_str(), resLen);
					if (sent != resLen) {	
						std::cout << "Sent : " << sent << "Bytes" << "Buffer had :" << resLen << "Bytes \n";
					}
					std::cout << "Response sent\n";
					return;
				}
			}
		}

		void handleError(CustomHTTPReq *req, StreamSocket& ss, int bytesRec) {
			if (req->getVerb() == std::string("POST")) {
				std::unordered_map<std::string, std::string> headers = req->getHeaders();
				int contentLength = std::stoi(headers["Content-Length"]);
				//std::cout << "\nContent length is " << contentLength << " req body = " << req->getBody().size() << "\n";
				if (req->getBody().size() != contentLength) {
					char buffer[contentLength];
					int n = ss.receiveBytes(buffer, sizeof(buffer));
					req->setBody(buffer);
					std::cout << " Fixed buffer, " << buffer;
					return;
				}
			}
		}

		void run()
		{
			StreamSocket& ss = socket();
			try
			{
				char buffer[8192];
				int n = ss.receiveBytes(buffer, sizeof(buffer));
				CustomHTTPReq *request = new CustomHTTPReq();
				while (n > 0)
				{ 
					std::string msg;
					Logger::formatDump(msg, buffer, n);
					std::cout << msg << std::endl;
					std::cout << "Received " << n << " bytes:" << std::endl;
					std::string http(buffer); 
					if (request->parseRequest(http)) {
						//std::cout <<  "Parsed request\n";
						std::cout << request->getVerb().c_str();
						handleError(request, ss, n);
						std::cout << "\nHandled error\n";
						handleConnection(request, ss);
						std::cout << "Im back\n";
					}
					else {

					}
					//request.printReqDetails();
					n = ss.receiveBytes(buffer, sizeof(buffer));
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