#pragma once	
#include <memory>
#include "Core/Subsystem/Context.h"
#include "Util/EventSystem.h"

namespace Framework
{
	class ISubsystem
	{
	public:
		ISubsystem(class Context* context) : _context(context) {};
		virtual ~ISubsystem() {};

		virtual bool Init() = 0;
		virtual void Update() = 0;

	protected:
		class Context* _context;
	};
}