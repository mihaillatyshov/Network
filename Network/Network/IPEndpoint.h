#pragma once

#include "IPVersion.h"
#include <string>
#include <vector>
#include <WS2tcpip.h>

namespace LM::Net
{

	class IPEndpoint
	{
	public:
		IPEndpoint(const char* ip, unsigned short port);
		IPEndpoint(sockaddr* addr);
		IPVersion GetIPVersion();
		std::vector<uint8_t> GetIPBytes();
		std::string GetHostname();
		std::string GetIPString();
		unsigned short GetPort();
		sockaddr_in GetSockaddrIPv4();
		sockaddr_in6 GetSockaddrIPv6();
		void Print();
	protected:
		//void ProcessIPv4();
		//void ProcessIPv6();
	protected:
		IPVersion m_IPVersion = IPVersion::Unknown;
		std::string m_Hostname = "";
		std::string m_IPString = "";
		std::vector<uint8_t> m_IPBytes;
		unsigned short m_Port = 0;
	};

}