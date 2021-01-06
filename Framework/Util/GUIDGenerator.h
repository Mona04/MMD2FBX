#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <combaseapi.h>
#pragma comment(lib, "Rpcrt4.lib")

namespace Framework
{
	class GUIDGenerator final
	{
	public:
		static const unsigned int GetIntCode()
		{
			// GUID 를 string 으로 바꾼걸 해쉬화함. 크기가 줄어드는게 다일듯.
			std::hash<std::string> hasher;
			return static_cast<unsigned int>(hasher(GetGUID()));
		}

	private:
		// 16 bit guid
		static std::string GetGUID()
		{
			GUID guid;	
			auto hr = UuidCreate(&guid);
			if (SUCCEEDED(hr))
			{
				std::stringstream sstream;
				sstream << std::hex << std::uppercase
					<< std::setw(8) << std::setfill('0') << guid.Data1 << "-"
					<< std::setw(4) << std::setfill('0') << guid.Data2 << "-"
					<< std::setw(4) << std::setfill('0') << guid.Data3 << "-"
					<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[0])
					<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[1])
					<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[2])
					<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[3])
					<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[4])
					<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[5])
					<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[6])
					<< std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[7]);
				return sstream.str();
			}

			return "N/A";
		}

	};
}