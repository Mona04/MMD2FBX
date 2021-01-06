#pragma once
#include <string>
#include "Util/GUIDGenerator.h"

namespace Framework
{
	class Chimera_Object
	{
	public:
		Chimera_Object() : _code(GUIDGenerator::GetIntCode()), _name("Default_Name")
		{
			
		}
		virtual ~Chimera_Object() = default;

		unsigned int GetCode() { return _code; }
		std::string GetName() { return _name; }
		void SetName(std::string_view name) { _name = name; }

	private:
		unsigned int _code;
		std::string _name;
	};
}