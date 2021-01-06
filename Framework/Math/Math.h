#pragma once
#include "Matrix.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Color4.h"
#include "Matrix_V.h"

class Math
{
public:
	static double TO_DEG;
	static double pi;
	static constexpr const double ToRadian(double euler) { return euler / 180.0f * pi; }
	static const Vector3 ToRadian(const Vector3& value) { return value * (pi / 180.0f); }

	template<typename T> 
	static const int Sign(const T& value) { return (T(0) < value) - (T(0) > value);}
	
	template<typename T>
	static const T Clamp(const T& value, const T& min, const T& max) { return value < min ? min : value > max ? max : value; }

	// when the vector values be absolute value, return the biggest absolute value ( > 0 )
	static float Max_AbsoluteMember(const Vector3& from);
	static Vector3 Absolute(const Vector3& from);

	static Quaternion Slerp(const Quaternion& from, const Quaternion& to, double labmda);
	static float Lerp(const float& from, const float& to, double lambda);
	static Vector2 Lerp(const Vector2& from, const Vector2& to, double lambda);
	static Vector3 Lerp(const Vector3& from, const Vector3& to, double lambda);
	static Quaternion Lerp(const Quaternion& from, const Quaternion& to, double lambda);
	static Vector2 Bezier3(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, double lambda);
	
	// make bound box with pos and offset, and calculate is from in the bound box;
	// no rotation is regarded
	static bool InBox(const Vector3& from, const Vector3& center, const Vector3& offset);
	// make bound sphere with pos and offset, and calculate is from in the bound sphere;
	// no rotation is regarded
	static bool InEllipse(const Vector3& from, const Vector3& center, const Vector3& offset);
};