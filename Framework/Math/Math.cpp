#include "Framework.h"
#include "Math.h"

double Math::pi = 3.141592653589793238462643383279502884197169399375105820974944;
double Math::TO_DEG = 180 / Math::pi;

float Math::Max_AbsoluteMember(const Vector3& from)
{
	Vector3 tmp = Absolute(from);
	if (tmp.x > tmp.y)
		return tmp.x > tmp.z ? tmp.x : tmp.y > tmp.z ? tmp.y : tmp.z;
	else {
		return tmp.y > tmp.z ? tmp.y : tmp.x > tmp.z ? tmp.x : tmp.z;
	}
}

Vector3 Math::Absolute(const Vector3& from)
{
	return Vector3(
		from.x > 0 ? from.x : -from.x,
		from.y > 0 ? from.y : -from.y,
		from.z > 0 ? from.z : -from.z
	);
}

float Math::Lerp(const float& from, const float& to, double lambda)
{
	return from * (1 - lambda) + to * lambda;
}

Vector2 Math::Lerp(const Vector2& from, const Vector2& to, double lambda)
{
	return from * (1 - lambda) + to * lambda;
}

Vector3 Math::Lerp(const Vector3& from, const Vector3& to, double lambda)
{
	return from * (1 - lambda) + to * lambda;
}

Quaternion Math::Lerp(const Quaternion& from, const Quaternion& to, double lambda)
{
	Quaternion result;
	result.x = from.x * (1-lambda) + to.x * (lambda);
	result.y = from.y * (1-lambda) + to.y * (lambda);
	result.z = from.z * (1-lambda) + to.z * (lambda);
	result.w = from.w * (1-lambda) + to.w * (lambda);
	return result;
}

Vector2 Math::Bezier3(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, double lambda)
{
	Vector2 pp0 = Lerp(p0, p1, lambda);
	Vector2 pp1 = Lerp(p1, p2, lambda);
	Vector2 pp2 = Lerp(p2, p3, lambda);
	return Lerp(Lerp(pp0, pp1, lambda), Lerp(pp1, pp2, lambda), lambda);
}


Quaternion Math::Slerp(const Quaternion& q1, const Quaternion& q2, double lambda)
{
	float theta = acos(Clamp(q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w, -1.f, 1.f));
	if (theta == 0)
		return q1;

	float sn = sin(theta);
	float Wa = sin((1 - lambda) * theta) / sn;
	float Wb = sin(lambda * theta) / sn;
	return Quaternion(
		Wa * q1.x + Wb * q2.x,
		Wa * q1.y + Wb * q2.y,
		Wa * q1.z + Wb * q2.z,
		Wa * q1.w + Wb * q2.w
	).Normalize();
}

bool Math::InBox(const Vector3& from, const Vector3& center, const Vector3& offset)
{
	Vector3 offset_absolute = Math::Absolute(offset);
	if (from.x > center.x + offset_absolute.x || from.x < center.x - offset_absolute.x)
		return false;
	if (from.y > center.y + offset_absolute.y || from.y < center.y - offset_absolute.y)
		return false;
	if (from.z > center.z + offset_absolute.z || from.z < center.z - offset_absolute.z)
		return false;
	return true;
}

bool Math::InEllipse(const Vector3& from, const Vector3& center, const Vector3& offset)
{
	if(	std::powf(from.x - center.x, 2) / (offset.x * offset.x)
		+ std::powf(from.y - center.y, 2) / (offset.y * offset.y)
		+ std::powf(from.z - center.z, 2) / (offset.z * offset.z) < 1)
		return true;
	return false;
}