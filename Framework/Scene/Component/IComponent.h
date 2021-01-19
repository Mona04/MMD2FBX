#pragma once
#include <memory>
#include <vector>

#include "../Chimera_Object.h"

#include "Framework/Core/Subsystem/Context.h"
#include "Framework/Core/Subsystem/Resource/ResourceManager.h"

#include "Framework/Math/Math.h"

namespace Framework
{
	class IComponent : public Chimera_Object
	{
	public:
		enum class TypeCode : unsigned int {
			None, Setting, Camera, Transform, Renderable, Animator, IKSolver,
			Collider, Light, Scripter, RigidBodys
		};
		template<typename T> static constexpr TypeCode DeduceType();

	public:
		IComponent(class Context* context) : Chimera_Object(), _context(context), _actor(nullptr), _resourceManager(context->GetSubsystem<ResourceManager>()) {};
		virtual ~IComponent() = default;

		virtual bool Update() = 0;
		virtual void Clear() = 0;

		class Context* GetContext() { return _context; }
		const TypeCode GetType() const { return typecode; }
		// Actor 의 AddComponent 에서 처리하니 걱정말라굿
		void SetActor(class Actor* actor) { _actor = actor; }
		const std::string GetTypeName() const {
			switch (typecode)
			{
			case TypeCode::None: return "None";
			case TypeCode::Setting: return "Setting";
			case TypeCode::Transform: return "Transform";
			case TypeCode::Renderable: return "Renderable";
			case TypeCode::Light: return "Light";
			case TypeCode::Scripter: return "Scripter";
			case TypeCode::Camera: return "Camera";
			case TypeCode::Animator: return "Animator";
			case TypeCode::Collider: return "Collider";
			case TypeCode::IKSolver: return "IKSolver";
			case TypeCode::RigidBodys: return "RigidBodys";
			default:
				return "None";
			}
		}

	protected:
		TypeCode typecode = TypeCode::None;
		class Context* _context;
		class ResourceManager* _resourceManager;
		class Actor* _actor;
	};

	template<typename T>
	inline constexpr IComponent::TypeCode IComponent::DeduceType() { return TypeCode::None; }

#define RegisterComponent(T, Code) template<> inline constexpr IComponent::TypeCode IComponent::DeduceType<T>() { return Code; }
	RegisterComponent(class Setting_Actor, TypeCode::Setting)
		RegisterComponent(class Transform, TypeCode::Transform)
		RegisterComponent(class Renderable, TypeCode::Renderable)
		RegisterComponent(class Light, TypeCode::Light)
		RegisterComponent(class Scripter, TypeCode::Scripter)
		RegisterComponent(class Camera, TypeCode::Camera)
		RegisterComponent(class Animator, TypeCode::Animator)
		RegisterComponent(class Collider, TypeCode::Collider)
		RegisterComponent(class IKSolver, TypeCode::IKSolver)
		RegisterComponent(class RigidBodys, TypeCode::RigidBodys)
}