#pragma once

#include "SocketHandle.h"
#include "NResult.h"
#include "IPVersion.h"
#include "SocketOption.h"
#include "IPEndpoint.h"
#include "Constants.h"
#include "Packet.h"

namespace LM::Net
{

	class Socket
	{
	public:
		Socket(IPVersion ipversion = IPVersion::IPv4, SocketHandle handle = INVALID_SOCKET);
		NResult Create();
		NResult Close();
		NResult Bind(IPEndpoint endpoint);
		NResult Listen(IPEndpoint endpoint, int backlog = 5);
		NResult Accept(Socket& outSocket);
		NResult Connect(IPEndpoint endpoint);
		NResult Send(const void* data, int numberOfBytes, int& bytesSent);
		NResult Recv(void* destination, int numberOfBytes, int& bytesReceived);
		NResult SendAll(const void* data, int numberOfBytes);
		NResult RecvAll(void* destination, int numberOfBytes);
		NResult Send(Packet& packet);
		NResult Recv(Packet& packet);
		SocketHandle GetHandle();
		IPVersion GetIPVersion();
	protected:
		NResult SetSocketOption(SocketOption option, BOOL value);
		IPVersion m_IPVersion = IPVersion::IPv4;
		SocketHandle m_Handle = INVALID_SOCKET;
	};

}