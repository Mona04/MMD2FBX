#include "Framework.h"
#include "Actor.h"
#include "Component/Transform.h"
#include "Component/Renderable.h"
#include "Component/Light.h"
#include "Component/Camera.h"
#include "Component/Animator.h"


using namespace Framework;

Actor::Actor(class Context* context) : Chimera_Object(), _context(context)
{
	// Transform 없으면 사람임? 
	AddComponent<Transform>();
}

Actor::~Actor()
{
	// Actor, Transform should be used in editor which run on mananged language that we cannot use smart ptr
	for (auto& component : _components)
		SAFE_DELETE(component);
}


bool Actor::Update()
{
	for (auto& component : _components)
		component->Update();
	
	return true;
}

void Actor::Init_Sample()
{
	AddComponent<Renderable>();
	AddComponent<Light>();
	GetComponent<Transform>()->SetPosition(Vector3(0.5, 0.5, 5));
}

void Actor::Init_Camera()
{
	AddComponent<Camera>();
}

void Actor::Init_Bone_Position()
{	
	AddComponent<Renderable>()->SetCube(); //L"_Assets/Model/Cube.mesh"
}

void Actor::Init_Gizmo()
{
	AddComponent<Renderable>()->SetGizmo(); //L"_Assets/Model/Cube.mesh"
	GetComponent<Transform>()->SetScale(Vector3(0.2f, 0.2f, 0.2f));
}

void Actor::Init_Grid()
{
	AddComponent<Renderable>()->SetGrid(); //L"_Assets/Model/Cube.mesh"
	GetComponent<Transform>()->SetScale(Vector3(0.2f, 0.2f, 0.2f));
}
