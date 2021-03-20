// Server

#include <Network/Includes.h>
#include <iostream>

bool ProcessPacket(LM::Net::Packet& packet)
{
	switch (packet.GetPacketType())
	{
	case LM::Net::PacketType::PT_ChatMessage:
	{
		std::string chatmessage;
		packet >> chatmessage;
		std::cout << "Chat Message: " << chatmessage << std::endl;
		break;
	}
	case LM::Net::PacketType::PT_IntegerArray:
	{
		uint32_t arraySize = 0;
		packet >> arraySize;
		std::cout << "Array size: " << arraySize << std::endl;
		for (uint32_t i = 0; i < arraySize; i++)
		{
			uint32_t element = 0;
			packet >> element;
			std::cout << "Element[" << i << "]: " << element << std::endl;
		}
		break;
	}
	default:
		return false;
	}


	return true;
}

int main()
{
	if (LM::Net::Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized" << std::endl;

		LM::Net::Socket socket(LM::Net::IPVersion::IPv6);
		if (socket.Create() == LM::Net::NResult::N_Success)
		{
			std::cout << "Socket successfully created" << std::endl;
			if (socket.Listen(LM::Net::IPEndpoint("::", 4790)) == LM::Net::NResult::N_Success)
			{
				std::cout << "Socket successfully listening on port 4790" << std::endl;
				LM::Net::Socket newConnection;
				if (socket.Accept(newConnection) == LM::Net::NResult::N_Success)
				{
					std::cout << "New connection accepted" << std::endl;

					LM::Net::Packet packet;
					while (true)
					{
						LM::Net::NResult result = newConnection.Recv(packet);
						if (result != LM::Net::NResult::N_Success)
							break;

						if (!ProcessPacket(packet))
							break;
					}

					newConnection.Close();
				}
				else
				{
					std::cerr << "Failed to accept new connection" << std::endl;
				}
			}
			else
			{
				std::cerr << "Failed to listen socket on port 4790" << std::endl;
			}
			socket.Close();
		}
		else
		{
			std::cerr << "Socket failed to create" << std::endl;
		}
	}

	LM::Net::Network::Shutdown();

	system("pause");
	return 0;
}