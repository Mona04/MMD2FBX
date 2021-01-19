#include "Framework.h"
#include "Physics_Ect.h"


using namespace Framework;

void PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
}

void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
{
	OutputDebugStringA(warningString);
}
