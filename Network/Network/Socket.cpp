#include "Socket.h"
#include <cassert>
#include <iostream>

namespace LM::Net
{

	Socket::Socket(IPVersion ipversion, SocketHandle handle)
		: m_IPVersion(ipversion), m_Handle(handle)
	{
		assert(m_IPVersion == IPVersion::IPv4 || m_IPVersion == IPVersion::IPv6);
	}

	NResult Socket::Create()
	{
		assert(m_IPVersion == IPVersion::IPv4 || m_IPVersion == IPVersion::IPv6);
		if (m_Handle != INVALID_SOCKET)
		{
			return NResult::N_GenericError;
		}

		m_Handle = socket(m_IPVersion == IPVersion::IPv4 ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_TCP);

		if (m_Handle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return NResult::N_GenericError;
		}

		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != NResult::N_Success)
		{
			return NResult::N_GenericError;
		}

		return NResult::N_Success;

	}

	NResult Socket::Close()
	{
		if (m_Handle != INVALID_SOCKET)
		{
			return NResult::N_GenericError;
		}

		int result = closesocket(m_Handle);
		if (result != 0)
		{
			int error = WSAGetLastError();
			return NResult::N_GenericError;
		}

		m_Handle = INVALID_SOCKET;
		return NResult::N_Success;
	}

	NResult Socket::Bind(IPEndpoint endpoint)
	{
		assert(m_IPVersion == endpoint.GetIPVersion());

		if (m_IPVersion == IPVersion::IPv4)
		{
			sockaddr_in addr = endpoint.GetSockaddrIPv4();
			int result = bind(m_Handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
			if (result != 0)
			{
				int error = WSAGetLastError();
				return NResult::N_GenericError;
			}
		}
		else
		{
			sockaddr_in6 addr = endpoint.GetSockaddrIPv6();
			int result = bind(m_Handle, (sockaddr*)(&addr), sizeof(sockaddr_in6));
			if (result != 0)
			{
				int error = WSAGetLastError();
				return NResult::N_GenericError;
			}
		}
		return NResult::N_Success;
	}

	NResult Socket::Listen(IPEndpoint endpoint, int backlog)
	{
		if (m_IPVersion == IPVersion::IPv6)
		{
			if (SetSocketOption(SocketOption::IPB6_Only, FALSE) != NResult::N_Success)
			{
				return NResult::N_GenericError;
			}
		}

		if (Bind(endpoint) != NResult::N_Success)
		{
			return NResult::N_GenericError;
		}

		int result = listen(m_Handle, backlog);
		if (result != 0)
		{
			int error = WSAGetLastError();
			return NResult::N_GenericError;
		}

		return NResult::N_Success;
	}

	NResult Socket::Accept(Socket& outSocket)
	{
		assert(m_IPVersion == IPVersion::IPv4 || m_IPVersion == IPVersion::IPv6);

		if (m_IPVersion == IPVersion::IPv4)
		{
			sockaddr_in addr = {};
			int len = sizeof(sockaddr_in);
			SocketHandle acceptedConnectionHandle = accept(m_Handle, (sockaddr*)(&addr), &len);
			if (acceptedConnectionHandle == INVALID_SOCKET)
			{
				int error = WSAGetLastError();
				return NResult::N_GenericError;
			}
			IPEndpoint newConnectionEndpoint((sockaddr*)&addr);
			std::cout << "New Connection accepted!" << std::endl;
			newConnectionEndpoint.Print();
			outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);
		}
		else
		{
			sockaddr_in6 addr = {};
			int len = sizeof(sockaddr_in6);
			SocketHandle acceptedConnectionHandle = accept(m_Handle, (sockaddr*)(&addr), &len);
			if (acceptedConnectionHandle == INVALID_SOCKET)
			{
				int error = WSAGetLastError();
				return NResult::N_GenericError;
			}
			IPEndpoint newConnectionEndpoint((sockaddr*)&addr);
			std::cout << "New Connection accepted!" << std::endl;
			newConnectionEndpoint.Print();
			outSocket = Socket(IPVersion::IPv6, acceptedConnectionHandle);
		}
		return NResult::N_Success;
	}

	NResult Socket::Connect(IPEndpoint endpoint)
	{
		assert(m_IPVersion == endpoint.GetIPVersion());

		int result = 0;
		if (m_IPVersion == IPVersion::IPv4)
		{
			sockaddr_in addr = endpoint.GetSockaddrIPv4();
			result = connect(m_Handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		}
		else
		{
			sockaddr_in6 addr = endpoint.GetSockaddrIPv6();
			result = connect(m_Handle, (sockaddr*)(&addr), sizeof(sockaddr_in6));
		}
		if (result != 0)
		{
			int error = WSAGetLastError();
			return NResult::N_GenericError;
		}

		return NResult::N_Success;
	}

	NResult Socket::Send(const void* data, int numberOfBytes, int& bytesSent)
	{
		bytesSent = send(m_Handle, (const char*)data, numberOfBytes, NULL);

		if (bytesSent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return NResult::N_GenericError;
		}

		return NResult::N_Success;
	}

	NResult Socket::Recv(void* destination, int numberOfBytes, int& bytesReceived)
	{
		bytesReceived = recv(m_Handle, (char*)destination, numberOfBytes, NULL);

		if (bytesReceived == 0)
		{
			return NResult::N_GenericError;
		}
		
		if (bytesReceived == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return NResult::N_GenericError;
		}

		return NResult::N_Success;
	}

	NResult Socket::SendAll(const void* data, int numberOfBytes)
	{
		int totalBytesSent = 0;

		while (totalBytesSent < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesSent;
			int bytesSent = 0;
			char* bufferOffset = (char*)data + totalBytesSent;
			NResult result = Send(bufferOffset, bytesRemaining, bytesSent);
			if (result != NResult::N_Success)
			{
				return NResult::N_GenericError;
			}
			totalBytesSent += bytesSent;
		}

		return NResult::N_Success;
	}

	NResult Socket::RecvAll(void* destination, int numberOfBytes)
	{
		int totalBytesReceived = 0;

		while (totalBytesReceived < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesReceived;
			int bytesSent = 0;
			char* bufferOffset = (char*)destination + totalBytesReceived;
			NResult result = Recv(bufferOffset, bytesRemaining, bytesSent);
			if (result != NResult::N_Success)
			{
				return NResult::N_GenericError;
			}
			totalBytesReceived += bytesSent;
		}

		return NResult::N_Success;
	}

	NResult Socket::Send(Packet& packet)
	{
		uint16_t encodedPacketSize = htons(packet.Buffer.size());
		NResult result = SendAll(&encodedPacketSize, sizeof(uint16_t));
		if (result != NResult::N_Success)
			return NResult::N_GenericError;

		result = SendAll(packet.Buffer.data(), packet.Buffer.size());
		if (result != NResult::N_Success)
			return NResult::N_GenericError;

		return NResult::N_Success;
	}

	NResult Socket::Recv(Packet& packet)
	{
		packet.Clear();

		uint16_t encodedSize = 0;
		NResult result = RecvAll(&encodedSize, sizeof(uint16_t));
		if (result != NResult::N_Success)
			return NResult::N_GenericError;

		uint16_t bufferSize = ntohs(encodedSize);

		if (bufferSize > g_MaxPacketSize)
			return NResult::N_GenericError;

		packet.Buffer.resize(bufferSize);
		result = RecvAll(&packet.Buffer[0], bufferSize);
		if (result != NResult::N_Success)
			return NResult::N_GenericError;
	
		return NResult::N_Success;
	}

	SocketHandle Socket::GetHandle()
	{
		return m_Handle;
	}

	IPVersion Socket::GetIPVersion()
	{
		return m_IPVersion;
	}

	NResult Socket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			result = setsockopt(m_Handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
			break;
		case SocketOption::IPB6_Only:
			result = setsockopt(m_Handle, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&value, sizeof(value));
			break;
		default:
			return NResult::N_GenericError;
		}

		if (result != 0)
		{
			int error = WSAGetLastError();
			return NResult::N_GenericError;
		}

		return NResult::N_Success;
	}

}