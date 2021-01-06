#include "Framework.h"
#include "Vector3.h"
#include "Math/Math.h"

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f)
{
}

Vector3::Vector3(float x) : x(x), y(x), z(x)
{
}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z)
{
}

bool Vector3::operator==(const Vector3& rhs) const
{
	return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
}

bool Vector3::operator!=(const Vector3& rhs) const
{
	return !(*this==rhs);
}

Vector3 Vector3::operator+(const Vector3 rhs) const
{
	return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
}

Vector3 Vector3::operator-(const Vector3 rhs) const
{
	return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
}

Vector3 Vector3::operator*(float rhs) const
{
	return Vector3(x * rhs, y * rhs, z * rhs);
}

Vector3 Vector3::operator*(const Vector3 rhs) const
{
	return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vector3 Vector3::operator*(const Matrix& rhs) const
{
	float new_x = (this->x * rhs._11) + (this->y * rhs._21) + (this->z * rhs._31) + rhs._41;
	float new_y = (this->x * rhs._12) + (this->y * rhs._22) + (this->z * rhs._32) + rhs._42;
	float new_z = (this->x * rhs._13) + (this->y * rhs._23) + (this->z * rhs._33) + rhs._43;
	float new_w = 1 / ((this->x * rhs._14) + (this->y * rhs._24) + (this->z * rhs._34) + rhs._44);

	return Vector3(new_x, new_y, new_z);
	//return Vector3(x * w, y * w, z * w);
}

void Vector3::operator*=(float rhs)
{
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
}

Vector3 operator-(const Vector3& rhs)
{
	return Vector3(-rhs.x, -rhs.y, -rhs.z);
}

Vector3 Vector3::Cross(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3(
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x
	);
}

float Vector3::Dot(const Vector3& lhs, const Vector3& rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

float Vector3::Vector3::Length() const
{
	return std::sqrtf(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize() const
{
	float length = Length();

	if (length == 0) return Vector3(0, 0, 0);

	return Vector3(x / length, y / length, z / length);
}

Quaternion Vector3::ToQuaternion() const
{
	return Quaternion::QuaternionFromEulerAngle(*this);
}
