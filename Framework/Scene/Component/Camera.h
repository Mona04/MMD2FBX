#pragma once
#include "IComponent.h"

namespace Framework
{
	class Camera : public IComponent
	{
	public:
		Camera(class Context* context);
		~Camera() = default;
		Camera(Camera& rhs) = delete;
		Camera(Camera&& rhs) = delete;
		Camera& operator=(Camera& rhs) = delete;
		Camera& operator=(Camera&& rhs) = delete;

		virtual bool Update() override;
		virtual void Clear() override;

		Matrix GetViewProj() { return m_proj * m_view; }
		Matrix GetView() { return m_view; }
		Matrix GetProj() { return m_proj; }

		Vector3 GetCameraPos() { return m_cameraPos; }
		float Get_Near() { return m_nearZ; }
		float Get_Far() { return m_farZ; }

	private:
		bool Update_View();
		bool Update_Proj();		

	private:	
		Matrix m_view;
		Matrix m_proj;

		float m_nearZ;
		float m_farZ;

		Vector3 m_cameraPos;
		Matrix m_rotation;
	};
}