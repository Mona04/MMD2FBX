#pragma once
#include <vector>
#include <functional>

namespace Framework
{
	#define EVENT_HANDLER(func) { [this]() { this->func(); } }
	class EventSystem final
	{
	public:
		enum class EventType : unsigned int { Init, Update, Render};
	private:
		EventSystem() = default;
		~EventSystem() = default;

	public:
		static EventSystem& GetInstance() { static EventSystem instance; return instance; }
		
		void Subscribe(EventType type, std::function<void()> func);
		void Fire(EventType type) { for (auto& func : m_funcs[type]) func(); }
		std::vector<std::function<void()>>& GetFunc(EventType type) { return m_funcs[type]; }

	protected:
		std::unordered_map<EventType, std::vector<std::function<void()>>> m_funcs;
	};
}