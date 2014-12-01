#include <WadeWork/Udp.h>

namespace ww
{
	namespace net
	{
		bool UDPSender::open(const char *ip, unsigned short port)
		{
			memset(_ipaddress,0,strlen(_ipaddress));
			sprintf(_ipaddress,"%s",ip);
			sprintf(_port,"%i",port);
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_DGRAM;
			
			printf("  IP: \"%s\"\n",_ipaddress);
			printf("PORT: \"%s\"\n",_port);
			
			if ((rv = getaddrinfo(_ipaddress, _port, &hints, &servinfo)) != 0) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return false;
			}
			
			// loop through all the results and make a socket
			for(p = servinfo; p != NULL; p = p->ai_next)
			{
				if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
				{
					perror("talker: socket");
					continue;
				}
				
				break;
			}
			
			const char yes = 1;
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
			{
				perror("setsockopt (SO_REUSEADDR)");
			}
			// this call is what allows broadcast packets to be sent:
			if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &yes,
						   sizeof yes) == -1) {
				perror("setsockopt (SO_BROADCAST)");
				//exit(1);
			}
			if (p == NULL)
			{
				printf("talker: failed to bind socket\n");
				return false;
			}
#if PLATFORM_WINDOWS
			u_long nonblockyes = 1;
			ioctlsocket(sockfd,FIONBIO,&nonblockyes);
#else
			int flags = fcntl(sockfd, F_GETFL, 0);
			fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
#endif
			return true;
		}
		bool UDPSender::send(Buffer *buffer)
		{
			//printf("udpsender::send\n");
			int total = 0;        // how many bytes we've sent
			int bytesleft = buffer->getlength(); // how many we have left to send
			int n = 0;
			while(total < buffer->getlength()) {
				//printf("bytesleft = %i, total = %i\n",bytesleft,total);
				n = ::sendto(sockfd,buffer->getbytes(), bytesleft, 0, p->ai_addr, p->ai_addrlen);
				if (n == -1) { break; }
				total += n;
				bytesleft -= n;
			}
			return n==-1?-1:total; // return -1 on failure, 0 on success
		}
		void UDPSender::close()
		{
			freeaddrinfo(servinfo);
#if PLATFORM_APPLE
			::close(sockfd);
#else
			closesocket(sockfd);
#endif
		}
		
		
		
		
		bool UDPReceiver::open(unsigned short port)
		{
			//printf("BEGIN UDPReceiver::open(%i)\n",port);
			sprintf(_port,"%i",port);
			
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_flags = AI_PASSIVE; // use my IP
			
			if ((rv = getaddrinfo(NULL, _port, &hints, &servinfo)) != 0)
			{
				//fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return false;
			}
			
			// loop through all the results and bind to the first we can
			for(p = servinfo; p != NULL; p = p->ai_next)
			{
				if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
				{
					//perror("listener: socket");
					continue;
				}
				
				if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
				{
#if PLATFORM_APPLE
					::close(sockfd);
#else
					closesocket(sockfd);
#endif
					//perror("listener: bind");
					continue;
				}
				
				break;
			}
			const char one = 1;
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) == -1)
			{
				perror("setsockopt (SO_REUSEADDR)");
			}
			if (p == NULL)
			{
				//fprintf(stderr, "listener: failed to bind socket\n");
				return false;
			}
#if PLATFORM_WINDOWS
			u_long nonblockyes = 1;
			ioctlsocket(sockfd,FIONBIO,&nonblockyes);
#else
			int flags = fcntl(sockfd, F_GETFL, 0);
			fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
#endif
			freeaddrinfo(servinfo);
			//printf("END UDPReceiver::open(%i)\n",port);
			return true;
		}
		Buffer *UDPReceiver::receive(char *their_ip)
		{
			Buffer *bob = new Buffer();
			char *data = new char[65536];
			int numbytes = 0;
			addr_len = sizeof their_addr;
			
			if ((numbytes = recvfrom(sockfd, data, 65535, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
			{
				//printf("nothing\n");
				//perror("recvfrom");
				//exit(1);
			}
			
			if (numbytes > 0)
			{
				bob->setbytes(data, numbytes);
				printf("------------------------ZOOF GABBER!------------------------\n");
				if (their_ip != NULL)
					printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family, &((struct sockaddr_in *)&their_addr)->sin_addr, their_ip, INET6_ADDRSTRLEN));
			}
			else
			{
				delete[] data;
				data = NULL;
				delete bob;
				bob = NULL;
			}
			delete[] data;
			return bob;
		}
		void UDPReceiver::close()
		{
#if PLATFORM_APPLE
			::close(sockfd);
#endif
		}
	} // namespace net
} // namespace ww
