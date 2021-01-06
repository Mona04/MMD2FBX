#include "Framework.h"
#include "Light.h"

#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"

using namespace Framework;

Light::Light(class Context* context)
	: IComponent(context), _diffuse(1), _direction(-1, -1, -1)
{
	typecode = TypeCode::Light;
}

Light::~Light()
{
}

bool Light::Update()
{

	return true;
}

void Light::Clear()
{
	_diffuse = 1;
	_direction = { -1, -1, -1 };
}
