#pragma once
#include <vector>
#include <memory>
#include "ISubsystem.h"

namespace Framework
{
	class Context 
	{
	public:
		Context() = default;
		virtual ~Context() = default;

		template<typename T>
		T* AddSubsystem() { 
			auto& result = m_subsystems.emplace_back(std::make_unique<T>(this)); 
			return static_cast<T*>(result.get());
		}

		template<typename T>
		T* GetSubsystem();

	protected:
		std::vector<std::unique_ptr<class ISubsystem>> m_subsystems;
	};


	template<typename T>
	inline T* Context::GetSubsystem()
	{
		for (auto& sub : m_subsystems)
		{
			if (typeid(*sub) == typeid(T))
				return static_cast<T*>(sub.get());
		}
		return nullptr;
	}
}