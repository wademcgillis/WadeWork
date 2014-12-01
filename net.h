#ifndef __WadeWork__net_h__
#define __WadeWork__net_h__
#include <WadeWork/sys_defines.h>
#if PLATFORM_APPLE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
//#include <CFNetwork/CFNetwork.h>
//#include <SystemConfigu
#else
//#include <winsock.h>
#include <windows.h>
#include <WS2tcpip.h>
#endif
#include <WadeWork/TCP.h>
#include <WadeWork/UDP.h>
#include <vector>
namespace ww
{
	namespace net
	{
		extern const char *uintToIP(unsigned int ip);
		extern void getAdapterIPs(std::vector<unsigned int> *vector);
	} // namespace net
} // namespace ww
#endif