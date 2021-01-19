#include "Framework.h"
#include "Setting_Actor.h"

using namespace Framework;

Setting_Actor::Setting_Actor(class Context* context)
	: IComponent(context), _use_ik(false), _use_physics(false)
{
	typecode = TypeCode::Setting;
}

bool Setting_Actor::Update()
{
	return true;
}
