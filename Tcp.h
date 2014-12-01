#ifndef __WadeWork__Tcp_h__
#define __WadeWork__Tcp_h__
#include <WadeWork/net.h>
#include <WadeWork/Buffer.h>
namespace ww
{
	namespace net
	{
		class TCPSocket
		{
		private:
			//
			struct addrinfo connectedSocketAddressInfo;
			//
			bool accepted;
			struct sockaddr acceptedSocketAddr;
			socklen_t acceptedSocketSize;
			int mysocket;
		public:
			TCPSocket(int sock, struct sockaddr info, int infosize);
			TCPSocket();
			void connect(const char *address, unsigned int port);
			bool connected();
			void close();
			bool send(Buffer *buffer);
			Buffer *receive();
			const char* getipaddress();
		};
		
		class TCPListener
		{
		private:
		public:
			int listenSocket;
			void listen(unsigned int port);
			TCPSocket *accept();
			void close();
		};
	} // namespace net
} // namespace ww
#endif