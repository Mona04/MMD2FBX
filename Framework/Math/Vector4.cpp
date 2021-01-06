#include "Framework.h"
#include "Vector4.h"
#include "Math/Math.h"

Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

Vector4::Vector4(float x) : x(x), y(x), z(x), w(x)
{
}

Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{
}

Vector4::Vector4(const Vector3& var, float w) : x(var.x), y(var.y), z(var.z), w(w)
{
}

bool Vector4::operator==(const Vector4& rhs) const
{
	return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w;
}

bool Vector4::operator!=(const Vector4& rhs) const
{
	return !(*this == rhs);
}

Vector4 Vector4::operator+(const Vector4 rhs) const
{
	return Vector4(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
}

Vector4 Vector4::operator-(const Vector4 rhs) const
{
	return Vector4(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
}

Vector4 Vector4::operator*(float rhs) const
{
	return Vector4(x * rhs, y * rhs, z * rhs, w * rhs);
}

Vector4 operator-(const Vector4& rhs)
{
	return Vector4(-rhs.x, -rhs.y, -rhs.z, -rhs.w);
}

float Vector4::Vector4::Length() const
{
	return std::sqrtf(x * x + y * y + z * z);
}