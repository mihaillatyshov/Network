#pragma once

#include <string>

namespace LM::Net
{

	class PacketException
	{
	public:
		PacketException(std::string exception)
			: m_Exception(exception)
		{}

		const char* what()
		{
			return m_Exception.c_str();
		}
		std::string ToString()
		{
			return m_Exception;
		}
	protected:
		std::string m_Exception;
	};

}