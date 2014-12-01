#ifndef __WadeWork__Udp_h__
#define __WadeWork__Udp_h__
#include <WadeWork/net.h>
#include <WadeWork/Buffer.h>
namespace ww
{
	namespace net
	{
		class UDPSender
		{
		private:
			char _ipaddress[32];
			char _port[32];
		private:
			struct addrinfo hints, *servinfo, *p;
			int rv;
			struct sockaddr_storage their_addr;
			socklen_t addr_len;
			int sockfd;
		public:
			bool open(const char *ip, unsigned short port);
			bool send(Buffer *buffer);
			void close();
		};
		
		class UDPReceiver
		{
		private:
			char *_ipaddress;
			char _port[32];
		private:
			struct addrinfo hints, *servinfo, *p;
			int rv;
			struct sockaddr_storage their_addr;
			socklen_t addr_len;
			int sockfd;
		public:
			bool open(unsigned short port);
			Buffer *receive(char *their_ip);
			void close();
		};
	} // namespace net
} // namespace ww
#endif