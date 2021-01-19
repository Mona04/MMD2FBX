#include "Framework.h"
#include "Actor.h"
#include "Component/Transform.h"
#include "Component/Animator.h"
#include "Component/Renderable.h"
#include "Component/Setting_Actor.h"


using namespace Framework;

Actor::Actor(class Context* context) : Chimera_Object(), _context(context)
{
	// Transform 없으면 사람임? 
	AddComponent<Transform>();
	_setting = AddComponent<Setting_Actor>();
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
	GetComponent<Transform>()->SetPosition(Vector3(0.5, 0.5, 5));
}