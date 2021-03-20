#include "IPEndpoint.h"
#include <cassert>
#include <iostream>

namespace LM::Net
{

	IPEndpoint::IPEndpoint(const char* ip, unsigned short port)
	{
		m_Port = port;

		// IPv4
		in_addr addr;
		int result = inet_pton(AF_INET, ip, &addr);

		if (result == 1)
		{
			if (addr.S_un.S_addr != INADDR_NONE)
			{
				m_IPString = ip;
				m_Hostname = ip;

				m_IPBytes.resize(sizeof(ULONG));
				memcpy(&m_IPBytes[0], &addr.S_un.S_addr, sizeof(ULONG));

				m_IPVersion = IPVersion::IPv4;
				return;
			}
		}

		addrinfo hints = {};
		hints.ai_family = AF_INET;
		addrinfo* hostinfo = nullptr;
		result = getaddrinfo(ip, NULL, &hints, &hostinfo);
		if (result == 0)
		{
			sockaddr_in *hostaddr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);

			m_IPString.resize(16);
			inet_ntop(AF_INET, &hostaddr->sin_addr, &m_IPString[0], 16);

			m_Hostname = ip;

			ULONG iplong = hostaddr->sin_addr.S_un.S_addr;
			m_IPBytes.resize(sizeof(ULONG));
			memcpy(&m_IPBytes[0], &iplong, sizeof(ULONG));

			m_IPVersion = IPVersion::IPv4;

			freeaddrinfo(hostinfo);
			return;
		}

		
		// IPv6
		in6_addr addr6;
		result = inet_pton(AF_INET6, ip, &addr6);

		if (result == 1)
		{
			m_IPString = ip;
			m_Hostname = ip;

			m_IPBytes.resize(16);
			memcpy(&m_IPBytes[0], &addr6.u, 16);

			m_IPVersion = IPVersion::IPv6;
			return;
		}

		addrinfo hintsv6 = {};
		hints.ai_family = AF_INET6;
		addrinfo* hostinfov6 = nullptr;
		result = getaddrinfo(ip, NULL, &hintsv6, &hostinfov6);
		if (result == 0)
		{
			sockaddr_in6* hostaddr = reinterpret_cast<sockaddr_in6*>(hostinfov6->ai_addr);

			m_IPString.resize(46);
			inet_ntop(AF_INET6, &hostaddr->sin6_addr, &m_IPString[0], 46);

			m_Hostname = ip;

			m_IPBytes.resize(16);
			memcpy(&m_IPBytes[0], &hostaddr->sin6_addr, 16);

			m_IPVersion = IPVersion::IPv6;

			freeaddrinfo(hostinfo);
			return;
		}
	}

	IPEndpoint::IPEndpoint(sockaddr* addr)
	{
		assert(addr->sa_family == AF_INET || addr->sa_family == AF_INET6);
		if (addr->sa_family == AF_INET)
		{
			sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);
			m_IPVersion = IPVersion::IPv4;
			m_Port = ntohs(addrv4->sin_port);
			m_IPBytes.resize(sizeof(ULONG));
			memcpy(&m_IPBytes[0], &addrv4->sin_addr, sizeof(ULONG));
			m_IPString.resize(16);
			inet_ntop(AF_INET, &addrv4->sin_addr, &m_IPString[0], 16);
			m_Hostname = m_IPString;
		}
		else
		{
			sockaddr_in6* addrv6 = reinterpret_cast<sockaddr_in6*>(addr);
			m_IPVersion = IPVersion::IPv6;
			m_Port = ntohs(addrv6->sin6_port);
			m_IPBytes.resize(16);
			memcpy(&m_IPBytes[0], &addrv6->sin6_addr, 16);
			m_IPString.resize(46);
			inet_ntop(AF_INET6, &addrv6->sin6_addr, &m_IPString[0], 46);
			m_Hostname = m_IPString;
		}
	}

	IPVersion IPEndpoint::GetIPVersion()
	{
		return m_IPVersion;
	}

	std::vector<uint8_t> IPEndpoint::GetIPBytes()
	{
		return m_IPBytes;
	}

	std::string IPEndpoint::GetHostname()
	{
		return m_Hostname;
	}

	std::string IPEndpoint::GetIPString()
	{
		return m_IPString;
	}

	unsigned short IPEndpoint::GetPort()
	{
		return m_Port;
	}

	sockaddr_in IPEndpoint::GetSockaddrIPv4()
	{
		assert(m_IPVersion == IPVersion::IPv4);
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr, &m_IPBytes[0], sizeof(ULONG));
		addr.sin_port = htons(m_Port);

		return addr;
	}

	sockaddr_in6 IPEndpoint::GetSockaddrIPv6()
	{
		assert(m_IPVersion == IPVersion::IPv6);
		sockaddr_in6 addr = {};
		addr.sin6_family = AF_INET6;
		memcpy(&addr.sin6_addr, &m_IPBytes[0], 16);
		addr.sin6_port = htons(m_Port);

		return addr;
	}

	void IPEndpoint::Print()
	{
		switch (m_IPVersion)
		{
		case IPVersion::IPv4:
			std::cout << "IP Version: IPv4" << std::endl;
			break;
		case IPVersion::IPv6:
			std::cout << "IP Version: IPv6" << std::endl;
			break;
		default:
			std::cout << "IP Version: Unknown" << std::endl;
			break;
		}


		std::cout << "Hostname: " << GetHostname() << std::endl;
		std::cout << "IP: " << GetIPString() << std::endl;
		std::cout << "Port: " << GetPort() << std::endl;
		std::cout << "IP Bytes: " << std::endl;
		for (auto& digit : GetIPBytes())
		{
			std::cout << (int)digit << std::endl;
		}
	}

}