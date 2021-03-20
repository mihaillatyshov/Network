#pragma once

#include <cstdint>

namespace LM::Net
{

	enum PacketType : uint16_t
	{
		PT_Invalid,
		PT_ChatMessage,
		PT_IntegerArray
	};

}