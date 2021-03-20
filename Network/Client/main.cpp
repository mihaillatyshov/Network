// Client

#include <Network/Includes.h>
#include <iostream>

int main()
{
	if (LM::Net::Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized" << std::endl;
		LM::Net::Socket socket(LM::Net::IPVersion::IPv6);
		if (socket.Create() == LM::Net::NResult::N_Success)
		{
			std::cout << "Socket successfully created" << std::endl;
			if (socket.Connect(LM::Net::IPEndpoint("::1", 4790)) == LM::Net::NResult::N_Success)
			{
				std::cout << "Successfully connected to server" << std::endl;

				LM::Net::Packet stringPacket(LM::Net::PT_ChatMessage);
				stringPacket << std::string("This is my string packet!");

				LM::Net::Packet integersPacket(LM::Net::PacketType::PT_IntegerArray);
				uint32_t arraySize = 6;
				uint32_t integerArray[6] = { 2, 5, 7, 1, 2, 9 };
				integersPacket << arraySize;
				for (auto integer : integerArray)
				{
					integersPacket << integer;
				}

				while (true)
				{
					LM::Net::NResult result;

					if (rand() % 2)
					{
						result = socket.Send(stringPacket);
					}
					else
					{
						result = socket.Send(integersPacket);
					}

					if (result != LM::Net::NResult::N_Success)
						break;

					std::cout << "Attemting to send chunk of data..." << std::endl;
					Sleep(500);
				}
			}
			else
			{
				std::cerr << "Failed to connect to server" << std::endl;
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