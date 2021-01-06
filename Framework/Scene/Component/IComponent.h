#pragma once
#include <memory>
#include <vector>

#include "../Chimera_Object.h"

#include "Core/Subsystem/Context.h"
#include "Core/Subsystem/Resource/ResourceManager.h"

#include "Math/Math.h"

namespace Framework
{
	class IComponent : public Chimera_Object
	{
	public:
		enum class TypeCode : unsigned int {
			None, Camera, Transform, Renderable, Animator, IKSolver,
			Collider, Light, Scripter,
		};
		template<typename T> static constexpr TypeCode DeduceType();

	public:
		IComponent(class Context* context) : Chimera_Object(), _context(context), _actor(nullptr), _resourceManager(context->GetSubsystem<ResourceManager>()) {};
		virtual ~IComponent() = default;

		virtual bool Update() = 0;
		virtual void Clear() = 0;

		const TypeCode GetType() const { return typecode; }
		// Actor �� AddComponent ���� ó���ϴ� ���������
		void SetActor(class Actor* actor) { _actor = actor; }
		const std::string GetTypeName() const {
			switch (typecode)
			{
			case TypeCode::None: return "None";
			case TypeCode::Transform: return "Transform";
			case TypeCode::Renderable: return "Renderable";
			case TypeCode::Light: return "Light";
			case TypeCode::Scripter: return "Scripter";
			case TypeCode::Camera: return "Camera";
			case TypeCode::Animator: return "Animator";
			case TypeCode::Collider: return "Collider";
			case TypeCode::IKSolver: return "IKSolver";
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
	RegisterComponent(class Transform, TypeCode::Transform)
	RegisterComponent(class Renderable, TypeCode::Renderable)
	RegisterComponent(class Light, TypeCode::Light)
	RegisterComponent(class Scripter, TypeCode::Scripter)
	RegisterComponent(class Camera, TypeCode::Camera)
	RegisterComponent(class Animator, TypeCode::Animator)
	RegisterComponent(class Collider, TypeCode::Collider)
	RegisterComponent(class IKSolver, TypeCode::IKSolver)
}