#pragma once
#include "IComponent.h"

namespace Framework
{
	class Light : public IComponent
	{
	public:
		Light(class Context* context);
		virtual ~Light();

		virtual bool Update() override;
		virtual void Clear() override;
		
		Vector3 Get_Diffuse() { return _diffuse; }
		void Set_Diffuse(const Vector3& var) { _diffuse = var; }
		Vector3 Get_Direction() { return _direction.Normalize(); }
		void Set_Direction(const Vector3& var) { _direction = var; }

	protected:
		Vector3 _diffuse;
		Vector3 _direction;
	};
}