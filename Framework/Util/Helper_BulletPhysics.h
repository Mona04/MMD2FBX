#pragma once

#include "bullet/btBulletCollisionCommon.h"
#include "bullet/btBulletDynamicsCommon.h"

#include "Math/Math.h"

namespace Helper_BulletPhysics
{
	inline btVector3 ToBtVec3(const Vector3& var)
	{
		return btVector3(var.x, var.y, var.z);
	}

	inline btQuaternion ToBtQuat(const Quaternion& var)
	{
		return btQuaternion(var.x, var.y, var.z, var.w);
	}

	inline Vector3 ToVec3(const btVector3& var)
	{
		return Vector3(var.x(), var.y(), var.z());
	}

	inline Quaternion ToQuat(const btQuaternion& var)
	{
		return Quaternion(var.x(), var.y(), var.z(), var.w());
	}
}