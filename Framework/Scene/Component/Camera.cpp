#include "Framework.h"
#include "Camera.h"

#include "Transform.h"
#include "../Actor.h"

using namespace Framework;

Camera::Camera(Context* context) 
    : IComponent(context), m_view(Matrix::identity), m_proj(Matrix::identity), m_cameraPos(0)
    , m_nearZ(0.1), m_farZ(1000)
{
    typecode = TypeCode::Camera;
}

bool Camera::Update()
{
    Update_View();
    Update_Proj();

	return true;
}

void Camera::Clear()
{
    m_view = Matrix::identity;
    m_proj = Matrix::identity;
    m_cameraPos = 0;
}

bool Camera::Update_View()
{
    auto transform = _actor->GetComponent<Transform>();
    auto rot = transform->GetRotation_V();

    m_cameraPos = transform->GetPosition();
    m_rotation = Matrix::QuaternionToMatrix(transform->GetRotation());

    m_view.LookAtUpLH(m_cameraPos, m_cameraPos + m_rotation * Vector3(0, 0, 1), m_rotation * Vector3(0, 1, 0));

    return true;
}

bool Camera::Update_Proj()
{    
    m_proj.PerspectiveFovLH(Math::pi / 2, 1.3f, m_nearZ, m_farZ);
    
    return true;
}
