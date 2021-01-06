#include "Framework.h"
#include "Vector2.h"
#include "Math/Math.h"

Vector2::Vector2() : x(0.0f), y(0.0f)
{
}

Vector2::Vector2(float x) : x(x), y(x)
{
}

Vector2::Vector2(float x, float y) : x(x), y(y)
{
}

bool Vector2::operator==(const Vector2& rhs) const
{
	return (this->x == rhs.y) && (this->y == rhs.y);
}

bool Vector2::operator!=(const Vector2& rhs) const
{
	return !(*this == rhs);
}

Vector2 Vector2::operator+(const Vector2 rhs) const
{
	return Vector2(this->x + rhs.x, this->y + rhs.y);
}

Vector2 Vector2::operator-(const Vector2 rhs) const
{
	return *this + (- rhs);
}

Vector2 Vector2::operator-() const
{
	return Vector2(-this->x, -this->y);
}

Vector2 Vector2::operator*(float rhs) const
{
	return Vector2(this->x * rhs, this->y * rhs);
}

Vector2 Vector2::operator*(const Vector2& rhs) const
{
	return Vector2(this->x * rhs.x, this->y * rhs.y);
}

float Vector2::Dot(const Vector2& lhs, const Vector2& rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y);
}

float Vector2::Length() const
{
	return std::sqrtf(x * x + y * y);
}

Vector2 Vector2::Normalize() const
{
	float length = Length();

	return Vector2(x / length, y / length);
}
