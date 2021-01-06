#include "Framework.h"
#include "EventSystem.h"

using namespace Framework;

void EventSystem::Subscribe(EventType type, std::function<void()> func)
{
	m_funcs[type].emplace_back(func);
}
