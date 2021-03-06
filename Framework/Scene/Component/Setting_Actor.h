#pragma once
#include "IComponent.h"

namespace Framework
{
	class Setting_Actor : public IComponent
	{
	public:
		Setting_Actor(class Context* context);
		virtual ~Setting_Actor() {}

		virtual bool Update() override;
		virtual void Clear() override {};

		void Set_UseIK(bool var) { _use_ik = var; }
		bool UseIK() { return _use_ik; }

		void Set_UsePhysics(bool var) { _use_physics = var; }
		bool UsePhysics() { return _use_physics; }

		void Set_UseJacobian(bool var) { _use_jacobian = var; }
		bool UseJacobian() { return _use_jacobian; }

	protected:
		bool _use_ik = true;
		bool _use_physics = false;
		bool _use_jacobian = false;
	};
}