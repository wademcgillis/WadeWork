#ifndef __WadeWork__Http_h__
#define __WadeWork__Http_h__
#include <WadeWork/net.h>
namespace ww
{
	namespace net
	{
		typedef struct
		{
			unsigned short status;
			std::string body;
		} HttpResponse;

		class HttpRequest
		{
		private:
			struct addrinfo connectedSocketAddressInfo;
			//
			bool accepted;
			struct sockaddr acceptedSocketAddr;
			socklen_t acceptedSocketSize;
			int mysocket;

			std::string host;
			std::string uri;
			std::string contentType;
			char *body;
			unsigned int bodyLength;
			unsigned int method;

			bool finished;
			bool async;
			HttpResponse response;
			unsigned int contentReceivedCount;
			unsigned int contentLength;

			void parseResponse(std::string *data, unsigned short *status, std::string *body, unsigned int *byteCount, unsigned int *bytesMax);

			void *threadPtr;
		public:
			static const unsigned char GET = 0;
			static const unsigned char POST = 1;
			HttpRequest();
			void setHost(std::string host);
			void setUri(std::string uri);
			void setBody(std::string body);
			void setBody(char *data, unsigned int length);
			void setMethod(unsigned char method);
			void setContentType(std::string contentType);
			void setAsync(bool async);
			void sendRequest(unsigned int timeoutish);

			unsigned int getContentReceivedCount();
			unsigned int getContentLength();

			bool getFinished();
			HttpResponse getResponse();
				
			void sendRequestFunc();
		};
	}
}
#endif