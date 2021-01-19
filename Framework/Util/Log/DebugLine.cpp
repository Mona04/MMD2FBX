#include "DebugLine.h"

void DebugLineQuaternion(const Quaternion& val)
{
	OutputDebugStringA(
		(std::to_string(val.x) + " " + std::to_string(val.y) + " "
			+ std::to_string(val.z) + " " + std::to_string(val.w) + "\n").c_str());
}

void DebugLineVector3(const Vector3& val)
{
	OutputDebugStringA(
		(std::to_string(val.x) + " " + std::to_string(val.y) + " "
			+ std::to_string(val.z) + "\n").c_str());
}