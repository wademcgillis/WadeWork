#include <WadeWork/Http.h>
#include <cmath>
#include <SFML/System/Thread.hpp>

namespace ww
{
	namespace net
	{
		HttpRequest::HttpRequest()
		{
			acceptedSocketSize = 0;
			mysocket = 0;
			host = "";
			uri = "";
			body = NULL;
			bodyLength = 0;
			method = -1;
			contentType = "";
		
			response.body = "";
			response.status = 0;		
			finished = false;
			sending = false;
			async = false;

			contentReceivedCount = 0;
			contentLength = 0;

			threadPtr = NULL;
		}

		void HttpRequest::clear()
		{
			acceptedSocketSize = 0;
			mysocket = 0;
			host = "";
			uri = "";
			body = NULL;
			bodyLength = 0;
			method = -1;
			contentType = "";
		
			response.body = "";
			response.status = 0;		
			finished = false;
			sending = false;
			async = false;

			contentReceivedCount = 0;
			contentLength = 0;

			threadPtr = NULL;
		}

		void HttpRequest::setHost(std::string _host)
		{
			host = _host;
		}

		void HttpRequest::setUri(std::string _uri)
		{
			uri = _uri;
		}

		void HttpRequest::setBody(std::string _body)
		{
			if (_body == "")
			{
				body = NULL;
				bodyLength = 0;
			}
			else
			{
				bodyLength = _body.length();
				delete body;
				body = new char[bodyLength+1];
				memset(body,0,bodyLength+1);
				memcpy(body,_body.c_str(),bodyLength);
			}
		}

		void HttpRequest::setBody(char *_body, unsigned int _length)
		{
			bodyLength = _length;
			delete body;
			body = new char[bodyLength+1];
			memset(body,0,bodyLength+1);
			memcpy(body,_body,bodyLength);
		}

		void HttpRequest::setMethod(unsigned char _method)
		{
			if (_method > 2)
				return;
			method = _method;
		}

		void HttpRequest::setContentType(std::string _contentType)
		{
			contentType = _contentType;
		}

		void HttpRequest::setAsync(bool _async)
		{
			async = _async;
		}

		unsigned int HttpRequest::getContentReceivedCount()
		{
			return contentReceivedCount;
		}
		unsigned int HttpRequest::getContentLength()
		{
			return contentLength;
		}

		void HttpRequest::parseResponse(std::string *data, unsigned short *__status, std::string *__body, unsigned int *__byteCount, unsigned int *__bytesMax)
		{
			if (!data)
				return;
			unsigned int headerEndPosition = 0;
			unsigned int bodyBeginPosition = 0;
			bool CRLF = true;
			headerEndPosition = data->find("\r\n\r\n");
			if (headerEndPosition == std::string::npos)
			{
				CRLF = false;
				headerEndPosition = data->find("\n\n");
				if (headerEndPosition == std::string::npos)
					return;
				bodyBeginPosition = headerEndPosition+2;
			}
			else
			{
				bodyBeginPosition = headerEndPosition+4;
			}

			if (__bytesMax)
			{
				std::string token = "Content-Length: ";
				unsigned int contentLengthPosition = data->find(token);
				if (contentLengthPosition != std::string::npos)
				{
					unsigned int contentLengthNewlinePosition = std::string::npos;
					if (CRLF)
						contentLengthNewlinePosition = data->find("\n",contentLengthPosition);
					else
						contentLengthNewlinePosition = data->find("\r\n",contentLengthPosition);
					if (contentLengthNewlinePosition != std::string::npos)
					{
						std::string contentlengthStr = data->substr(contentLengthPosition+token.length(),contentLengthNewlinePosition-(contentLengthPosition+token.length()+1));
						
						unsigned int tempLength = 0;
						for(int i=0;i<contentlengthStr.length();i++)
						{
							if (contentlengthStr.at(i) >= '0' && contentlengthStr.at(i) <= '9')
								tempLength += (contentlengthStr.at(i)-'0')*pow(10.f,(float)((contentlengthStr.length()-1)-i));
							else
							{
								tempLength = 0;
								break;
							}
						}

						if (__bytesMax)
							*__bytesMax = tempLength;

						if (__byteCount)
							*__byteCount = data->length()-bodyBeginPosition;
					}
				}
			}

			if (__status)
			{
				unsigned short tempStatus = 0;


				unsigned int temp1 = data->find(" ");
				unsigned int temp2 = 0;
				if (temp1 != std::string::npos)
				{
					temp2 = data->find(" ",temp1+1);
					if (temp2 != std::string::npos)
					{	
						std::string statusID = data->substr(temp1+1,temp2-(temp1+1));
						for(int i=0;i<statusID.length();i++)
						{
							if (statusID.at(i) >= '0' && statusID.at(i) <= '9')
								tempStatus += (statusID.at(i)-'0')*pow(10.f,(float)((statusID.length()-1)-i));
							else
							{
								tempStatus = 0;
								break;
							}
						}
					}
				}
				*__status = tempStatus;
			}

			if (__body)
				*__body = data->substr(bodyBeginPosition,data->length()-bodyBeginPosition);
		}
		void HttpRequest::sendRequestFunc()
		{
			printf("STARTING\n.\n.\n.\n.\n.\n.\n.\n.\nREQUEST\n");
			response.body = "";
			response.status = 0;
			contentReceivedCount = 0;
			contentLength = 0;



	//printf("connecting...\n");
			struct addrinfo *serverinfo;
			errno = 0;
			memset(&connectedSocketAddressInfo, 0, sizeof(addrinfo));
			connectedSocketAddressInfo.ai_family = AF_INET;
			connectedSocketAddressInfo.ai_socktype = SOCK_STREAM;
			connectedSocketAddressInfo.ai_protocol = IPPROTO_TCP;
			unsigned short port = 80;
			char port_cstr[6] = "";
			sprintf(port_cstr,"%i",port);
			errno = 0;
			int r;
			if ((r = getaddrinfo(host.c_str(),port_cstr,&connectedSocketAddressInfo, &serverinfo)) != 0)
			{
				perror("getaddrinfo failure");
				errno = NULL;
			}
			
			const char yes = 1;
			struct addrinfo *walker;
			bool found = false;
			for(walker = serverinfo; walker != NULL; walker = walker->ai_next)
			{
				mysocket = socket(walker->ai_family, walker->ai_socktype, walker->ai_protocol);
				if (mysocket == -1)
				{
					perror("socket = -1 error");
					errno = NULL;
					continue;
				}
				int ret = ::connect(mysocket, walker->ai_addr, walker->ai_addrlen);
				if (ret == -1)
				{
					printf("TCPSocket::connect tried on socket %i, but we got error #%i\n",mysocket,errno);
					perror("perror");
					errno = NULL;
					continue;
				}
				if (setsockopt(mysocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
					perror("setsockopt failure");
				found = true;
				break;
			}
			freeaddrinfo(serverinfo);

			if (found)
			{
				//printf("connected.\n");
				//TCP_NODELAY
				setsockopt(mysocket, IPPROTO_TCP, /*TCP_NODELAY*/1, (char*)&yes, sizeof(int));
#if PLATFORM_WINDOWS
				u_long nonblocking = 0;
				ioctlsocket(mysocket,FIONBIO,&nonblocking);
#else
				int flags = fcntl(mysocket, F_GETFL, 0);
				fcntl(mysocket, F_SETFL, flags | O_NONBLOCK);
#endif
				errno = 0;

				std::string requestContents = "";
				requestContents += (method == POST?"POST ":"GET ")+uri+" HTTP/1.0\r\n";
				// HTTP 1.0 so the server doesn't respond with Chunked Transfer
				// encoding because I don't want to write code to support it. :)
				requestContents += "Host: "+host+"\r\n";
				requestContents += "Connection: close\r\n";
				if (body)
				{
					requestContents += "Content-Type: "+contentType+"\r\n";
					char text[32];
					memset(text,0,32);
					sprintf(text,"%i",bodyLength);
					requestContents += "Content-Length: "+std::string(text)+"\r\n";
					requestContents += "\r\n";
					requestContents.append(body,bodyLength);
					requestContents += "\r\n";
				}
				else
					requestContents += "\r\n";
				char *dataPtr = (char*)requestContents.c_str();
				int total = 0;
				unsigned int bytesToSend = requestContents.length();
				unsigned int bytesRemaining = requestContents.length();
				int n = 0;
				//printf("sending request...\n");
				while(total < bytesToSend)
				{
					n = send(mysocket, dataPtr+total, bytesRemaining, 0);
					if (n == -1) { break; }
					total += n;
					bytesRemaining -= n;
					//printf("sent %i bytes. total is now %i / %i\n",n,total,bytesToSend);
				}
				if (n == total)
				{
					std::string received;
					int total = 0;
					int n = 1;
					char temp[256];
					while(n != 0)
					{
						//printf("WHOOOOBOBOBOBOBBO\n");
						n = recv(mysocket, temp, sizeof(temp), 0);
						if (n == -1)
						{
							int errorcode = WSAGetLastError();
							switch(errorcode)
							{
							case WSAEWOULDBLOCK:  printf("ERROR: would block\n"); break;
							case WSAECONNABORTED:  printf("ERROR: connection aborted\n"); break;
							case WSAECONNRESET:  printf("ERROR: connection reset\n"); break;
							case WSAETIMEDOUT:  printf("ERROR: timeout\n"); break;
							case WSAENETRESET:  printf("ERROR: net reset\n"); break;
							case WSAENOTCONN:  printf("ERROR: not connected\n"); break;
							}
							//break;
						}
						else
						{
							received.append(temp,n);
							total += n;
							//printf("received %i bytes right now. %i received so far\n",n,total);

							parseResponse(&received,NULL,NULL,&contentReceivedCount,&contentLength);
						}
					}
					if (n == 0) // EVERYTHING SUCCEEDED
					{
						//printf("response received.\n");
						//printf("{%s}\n",received.c_str());
						parseResponse(&received,&response.status,&response.body,NULL,NULL);
					}
				} // n == total
				else
				{
					if (n == -1)
						printf("Failed to connect to server.\n");
					else
						printf("Failed to send all bytes, %i out of %i remain\n",total-bytesToSend,bytesToSend);
				}
			} // if (found)
			closesocket(mysocket);
			finished = true;
			sending = false;
		}

		bool HttpRequest::getFinished()
		{
			return finished;
		}
		HttpResponse HttpRequest::getResponse()
		{
			if (finished)
			{
				if (threadPtr)
				{
					((sf::Thread*)threadPtr)->terminate();
					delete threadPtr;
					threadPtr = NULL;
				}
				return response;
			}
			else
			{
				HttpResponse bob;
				bob.status = 0;
				bob.body = "";
				return bob;
			}
		}

		void HttpRequest::sendRequest(unsigned int timeoutish)
		{
			if (sending)
				return;
			sending = true;
			if (threadPtr)
			{
				((sf::Thread*)threadPtr)->terminate();
				delete threadPtr;
				threadPtr = NULL;
			}
			finished = false;
			if (async)
			{
				threadPtr = (void*)(new sf::Thread(&HttpRequest::sendRequestFunc,this));
				((sf::Thread*)threadPtr)->launch();
			}
			else
			{
				
				sendRequestFunc();
			}
		}
	}
}