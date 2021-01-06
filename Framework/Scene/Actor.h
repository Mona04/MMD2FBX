#pragma once
#include <vector>
#include "Component/IComponent.h"
#include "Chimera_Object.h"

namespace Framework
{
	class Actor : public Chimera_Object
	{
	public:
		Actor(class Context* context);
		virtual ~Actor();

		//Actor(Actor& rhs) = delete;
		//Actor(Actor&& rhs) = delete;
		//Actor& operator=(Actor& rhs) = delete;
		//Actor& operator=(Actor&& rhs) = delete;

		bool Update();
		void Init_Sample();
		void Init_Camera();
		void Init_Bone_Position();
		void Init_Gizmo();
		void Init_Grid();

		std::vector<class IComponent*> GetComponents() { return _components; }

		// Component 는 각 Actor 가 종류별로 하나 이하만 가짐. 여러개 가질려면 여러 액터를 조합해야할 것임.
		template<typename T> T* GetComponent();
		template<typename T> T* AddComponent();
		template<typename T> T* DeleteComponent();

		template<typename T>
		bool HasComponent() { return static_cast<bool>(GetComponent<T>()); }


	protected:
		class Context* _context;
		std::vector<class IComponent*> _components;
	};

	template<typename T>
	inline T* Actor::GetComponent()
	{
		static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent");
		IComponent::TypeCode type = IComponent::DeduceType<T>();

		auto result = std::find_if(_components.begin(), _components.end(),
				[type](auto var) { return type == var->GetType(); } );
		
		if (result == _components.end())
			return nullptr;

		return dynamic_cast<T*>(*result);
	}

	template<typename T>
	inline T* Actor::AddComponent()
	{
		static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent");

		if (HasComponent<T>())
		{
			LOG_WARNING("Already has component " + std::string(typeid(T).name()));
			return nullptr;
		}

		auto comp = _components.emplace_back(new T(_context));
		comp->SetActor(this);

		return dynamic_cast<T*>(comp);
	}
	template<typename T>
	inline T* Actor::DeleteComponent()
	{
		static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent");
		IComponent::TypeCode type = IComponent::DeduceType<T>();

		auto iter = std::find_if(_components.begin(), _components.end(),
			[type](auto var) { return type == var->GetType(); });

		if (iter == _components.end())
			return nullptr;

		auto result = *iter;
		_components.erase(iter);

		return dynamic_cast<T*>(result);
	}
}