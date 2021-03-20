#include "Network.h"

#include <iostream>

namespace LM::Net
{

    bool Network::Initialize()
    {
        WSADATA WSAData;
        int result = WSAStartup(MAKEWORD(2,2), &WSAData);
        if (result != 0)
        {
            std::cerr << "Failed to start up the winsock API" << std::endl;
            return false;
        }

        if (LOBYTE(WSAData.wVersion) != 2 || HIBYTE(WSAData.wVersion) != 2)
        {
            std::cerr << "Could not find a usavle version of the winsock api dll" << std::endl;
            return false;
        }

        return true;
    }

    void Network::Shutdown()
    {

    }

}