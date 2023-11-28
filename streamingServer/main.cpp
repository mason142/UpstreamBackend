#include "include/streamingServer.h"
#include "include/CustomHTTPReq.h"

namespace
{
	class ClientConnection: public TCPServerConnection
	{
	public:
		ClientConnection(const StreamSocket& s): TCPServerConnection(s)
		{
		}

		void run()
		{
			StreamSocket& ss = socket();
			try
			{
				char buffer[8192];
				int n = ss.receiveBytes(buffer, sizeof(buffer));
				while (n > 0)
				{ 
					std::cout << "Received " << n << " bytes:" << std::endl;
					//std::string http(buffer); 
					//CustomHTTPReq request;
					//request.parseRequest(http);
					//request.printReqDetails();
					std::string msg;
					Logger::formatDump(msg, buffer, n);
					std::cout << msg << std::endl;
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