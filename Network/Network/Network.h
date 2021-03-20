#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

namespace LM::Net
{

	class Network
	{
	public:
		static bool Initialize();
		static void Shutdown();
	protected:

	};

}